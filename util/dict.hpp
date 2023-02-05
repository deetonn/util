#pragma once

#include "common.h"
#include <type_traits>

#include "error.h"

_UTIL_API

template<typename _Ty>
struct hash;

#ifndef _DICT_INITIAL_SIZE
#define _DICT_INITIAL_SIZE 6
#endif

template<typename K, typename V>
struct pair {
    K key;
    V value;
};

template<typename _Kty, typename _Vty>
struct pair_view {
    _Kty& key;
    _Vty& value;
};

template<class K, class V>
std::ostream& operator <<(std::ostream& _Ostr, pair<K, V> pair) {
    _Ostr << "{" << pair.key << "," << pair.value << "}";
    return _Ostr;
}

template<typename T>
concept hashable = std::is_default_constructible_v<hash<T>>;

template<hashable _Key, collection_suitable _Val>
class dict {
public:
    using pair_type = pair<_Key, _Val>;
private:
    pair_type** m_nodes{ nullptr };

    size_t m_count{ 0 };
    size_t m_total_nodes{ 0 };

    _Val m_default{};
    pair_type m_default_pair{ _Key(), _Val() };
public:
    using pos_type = size_t;

    dict() {
        m_nodes
            = static_cast<pair_type**>(
                _STD malloc(_DICT_INITIAL_SIZE * sizeof(pair_type*)));
        m_count = 0;
        m_total_nodes = _DICT_INITIAL_SIZE;

        if (!m_nodes) {
            throw std::exception("failed to allocate initial space for dict");
        }

        _STD memset(m_nodes, NULL, _DICT_INITIAL_SIZE * sizeof(pair_type*));
    }

    dict(std::initializer_list<pair_type> _Initializer)
        : dict()
    {
        for (const pair_type& _Pair : _Initializer) {
            this->insert(_Pair.key, _Pair.value);
        }
    }

    pos_type insert(_Key const& key, _Val&& value) {
        auto _Hash = this->hash_item(key);
        _Ensure_capacity();
        m_nodes[_Hash] = new pair_type{ key, _STD move(value) };
        m_count++;
        return _Hash;
    }

    pos_type insert(_Key const& key, _Val const& value) {
        auto _Hash = this->hash_item(key);
        _Ensure_capacity();
        m_nodes[_Hash] = new pair_type{ key, value };
        m_count++;
        return _Hash;
    }

    pair_type const& get(_Key const& key) const {
        // dont hash twice (contains hashes too)
        auto _Hash = this->hash_item(key);
        if (!contains(_Hash)) {
            return m_default_pair;
        }
        return *m_nodes[_Hash];
    }

    pair_type& get(_Key const& key) {
        auto _Hash = this->hash_item(key);
        if (!contains(_Hash)) {
            return m_default_pair;
        }
        return *m_nodes[_Hash];
    }

    ErrorOr<int> remove(_Key const& key) {
        auto _Hash = this->hash_item(key);
        if (!contains(_Hash)) {
            return Error("no such key");
        }
        // get the value then free the memory
        auto value = _STD move(m_nodes[_Hash]->value);
        delete m_nodes[_Hash];
        m_nodes[_Hash] = nullptr;
        m_count--;
        return value;
    }

    size_t size() const noexcept {
        return m_count;
    }

    bool contains(_Key const& key) {
        auto _Hash = this->hash_item(key);
        return m_nodes[_Hash] != nullptr;
    }
    bool contains(size_t hash) {
        if (hash < 0 || hash > m_total_nodes)
            return false;
        return m_nodes[hash] != nullptr;
    }

    _Val& operator [](_Key const& key) {
        auto& ref = get(key);
        return ref.value;
    }

    // iterator
    class iterator {
    private:
        pair_type** m_iterator{ nullptr };
    public:
        iterator(pair_type** _Position)
            : m_iterator(_Position)
        {}

        using difference_type = uintptr_t;
        using value_type = pair_view<_Key, _Val>;
        using pointer = const pair_type*;
        using reference = const pair_type&;
        using iterator_category = std::forward_iterator_tag;

        iterator& operator ++() {
            m_iterator++;
            return *this;
        }
        iterator operator ++(int) {
            iterator self = *this;
            ++(*this);
            return self;
        }

        bool operator <(iterator const& other) {
            return m_iterator <= other.m_iterator;
        }
        bool operator >(iterator const& other) {
            return !(*this < other);
        }

        iterator operator +(size_t number) {
            return m_iterator + number;
        }

        //auto operator <=>(iterator const&) = default;

        bool operator==(iterator other) const { return this->m_iterator == other.m_iterator; }
        bool operator!=(iterator other) const { return !(*this == other); }

        value_type operator*() { 
            auto& _Actual = *(*m_iterator);
            return pair_view{ _Actual.key, _Actual.value };
        }
    };

    iterator begin() noexcept { return iterator(m_nodes); }
    iterator end() noexcept { return iterator(m_nodes + size()); }

private:

    void _Ensure_capacity() {
        auto _Capacity = (m_total_nodes);
        auto _Check = m_count + 1;

        if (_Check < _Capacity)
            return;

        auto _Newcap = (_Capacity + (_Capacity / 2));
        auto* _Space = _STD malloc(_Newcap + (sizeof(pair_type*)));

        if (!_Space) {
            throw std::exception("failed to allocate space to expand dict");
        }

        _STD memset(_Space, NULL, _Newcap * sizeof(pair_type*));

        _STD memmove(_Space, this->m_nodes, _Capacity);
        _STD free(this->m_nodes);

        this->m_nodes = static_cast<pair_type**>(_Space);
        this->m_total_nodes = _Newcap;
    }
    size_t hash_item(_Key const& _Item) {
        return hash<_Key>().generate(_Item, m_total_nodes);
    }
};

template<> struct hash<void*> {
public:
    size_t generate(const void* _Val, size_t const& _Max) {
        return reinterpret_cast<uintptr_t>(_Val) % _Max;
    }
};

template<> struct hash<int> {
public:
    size_t generate(int const& _Val, size_t const& _Max) {
        return _Val % _Max;
    }
};

template<> struct hash<size_t> {
public:
    size_t generate(size_t const& _Val, size_t const& _Max) {
        return _Val % _Max;
    }
};

template<> struct hash<std::string> {
public:
    size_t generate(std::string const& _Val, size_t const& _Max) {
        size_t _Final = 0;
        auto _Size = _Val.size();
        for (auto i = 0; i < _Size; ++i) {
            _Final += _Val[i];
        }
        return (_Final * _Size) % _Max;
    }
};

template<> struct hash<float> {
public:
    size_t generate(float const& _Val, size_t const& _Max) {
        return static_cast<size_t>(_Val) % _Max;
    }
};

template<hashable Key, collection_suitable Value>
using dict_pair = pair<Key, Value>;

template<hashable Key, collection_suitable Value>
using dict_mutator = std::function<dict_pair<Key, Value>(dict_pair<Key, Value>&)>;

template<hashable _Kty, collection_suitable _Vty>
dict<_Kty, _Vty> make_dict(
    std::initializer_list<dict_pair<_Kty, _Vty>> _Init
)
{
    return dict<_Kty, _Vty>(_Init);
}

template<hashable Key, collection_suitable Value>
dict<Key, Value> map(dict<Key, Value>& dict, dict_mutator<Key, Value> mutator) {
    for (auto& _Item : dict) {
        _Item = mutator(_Item);
    }
    return dict;
}

_UTIL_API_END