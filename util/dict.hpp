#pragma once

#include "common.h"

#include "error.h"

_UTIL_API

template<typename _Ty>
struct hash;

#ifndef _DICT_INITIAL_SIZE
#define _DICT_INITIAL_SIZE 6
#endif

template<typename T>
concept hashable = std::is_default_constructible_v<hash<T>>;

template<hashable _Key, collection_suitable _Val>
class dict {
private:
    _Val** m_nodes{ nullptr };

    size_t m_count{ 0 };
    size_t m_total_nodes{ 0 };

    _Val m_default{};
public:
    using pos_type = size_t;

    dict() {
        m_nodes
            = static_cast<_Val**>(
                _STD malloc(_DICT_INITIAL_SIZE * sizeof(_Val*)));
        m_count = 0;
        m_total_nodes = _DICT_INITIAL_SIZE;

        _STD memset(m_nodes, NULL, _DICT_INITIAL_SIZE * sizeof(_Val*));
    }

    pos_type insert(_Key const& key, _Val&& value) {
        auto _Hash = this->hash_item(key);
        _Ensure_capacity();
        m_nodes[_Hash] = new _Val(_STD move(value));
        m_count++;
        return _Hash;
    }

    _Val const& get(_Key const& key) const {
        // dont hash twice (contains hashes too)
        auto _Hash = this->hash_item(key);
        if (!contains(_Hash)) {
            return m_default;
        }
        return *m_nodes[_Hash];
    }

    _Val& get(_Key const& key) {
        auto _Hash = this->hash_item(key);
        if (!contains(_Hash)) {
            return m_default;
        }
        return *m_nodes[_Hash];
    }

    ErrorOr<int> remove(_Key const& key) {
        auto _Hash = this->hash_item(key);
        if (!contains(_Hash)) {
            return Error("no such key");
        }
        // get the value then free the memory
        auto value = _STD move(*m_nodes[_Hash]);
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
        return get(key);
    }
private:

    void _Ensure_capacity() {
        auto _Capacity = (m_total_nodes);
        auto _Check = m_count + 1;

        if (_Check < _Capacity)
            return;

        auto _Newcap = (_Capacity + (_Capacity / 2));
        auto* _Space = _STD malloc(_Newcap + (sizeof(_Val*)));

        _STD memset(_Space, NULL, _Newcap * sizeof(_Val*));

        _STD memmove(_Space, this->m_nodes, _Capacity);
        _STD free(this->m_nodes);

        this->m_nodes = static_cast<_Val**>(_Space);
        this->m_total_nodes = _Newcap;
    }
    size_t hash_item(_Key const& _Item) {
        return hash<_Key>().generate(_Item, m_total_nodes);
    }
};

template<> struct hash<void*> {
public:
    size_t generate(void* _Val, size_t const& _Max) {
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

_UTIL_API_END