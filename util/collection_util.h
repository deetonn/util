#pragma once

#include "common.h"
#include "memory_util.h"

#include <functional>

_UTIL_API

template<class T>
concept collection_suitable
        = !std::is_pointer_v<T>
        && !std::is_reference_v<T>
        && std::is_default_constructible_v<T>;

enum : size_t { Ok, OutOfSpace };

template<collection_suitable _Ty, size_t N>
class stack {
private:
    _Ty m_stack[N] = {};
    size_t m_position{ 0 };

    _Ty m_default{};
public:
    stack() = default;

    size_t push(_Ty&& _Value) {
        const auto _Next = m_position == 0 ? 0 : (m_position + 1);
        if (_Next >= N) {
            /*out of space*/
            return OutOfSpace;
        }
        m_stack[_Next] = _STD move(_Value);
        m_position++;
        return Ok;
    }
    size_t push(const _Ty& _Value) {
        const auto _Next = m_position == 0 ? 0 : (m_position + 1);
        if (_Next >= N) {
            /*out of space*/
            return OutOfSpace;
        }
        m_stack[_Next] = _Value;
        m_position++;
        return Ok;
    }
    _Ty pop() {
        if (m_position == 0) {
            return _Ty();
        }
        return _STD move(m_stack[--m_position]);
    }
    _Ty& peek() {
        if (m_position == 0) {
            return m_default;
        }
        return m_stack[m_position - 1];
    }

    size_t size() const noexcept {
        return m_position;
    }
    size_t empty() const noexcept { return size() == 0; }

    bool at_capacity() const noexcept {
        return size() == N;
    }

    /* iterator */
    class iterator {
    private:
        _Ty* m_iterator{ nullptr };
    public:
        iterator(_Ty* _Position)
            : m_iterator(_Position)
        {}

        using difference_type = long;
        using value_type = long;
        using pointer = const long*;
        using reference = const long&;
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

        bool operator==(iterator other) const { return this->m_iterator == other.m_iterator; }
        bool operator!=(iterator other) const { return !(*this == other); }

        _Ty& operator*() { return *m_iterator; }
    };

    iterator begin() noexcept { return iterator(m_stack); }
    iterator end() noexcept { return iterator(m_stack + m_position); }
};

#ifndef _VECTOR_INITIAL_SIZE
#define _VECTOR_INITIAL_SIZE 8
#endif

#ifndef _VECTOR_CACHE_SIZE 
#define _VECTOR_CACHE_SIZE 10
#endif

#define _THROW_OOM() throw std::exception("failed to allocate")

template<collection_suitable _Ty>
class vector {
private:
    _Ty* m_elements{ nullptr };
    size_t m_element_count{ 0 };
    size_t m_total_allocated_nodes{ 0 };

    stack<size_t, _VECTOR_CACHE_SIZE> m_cache{};

    _Ty m_default = {};
public:
    vector()
        : m_total_allocated_nodes(_VECTOR_INITIAL_SIZE)
    {
        m_elements
            = static_cast<_Ty*>(_STD calloc(_VECTOR_INITIAL_SIZE, sizeof(_Ty)));
        /*m_element_count = 0*/

        if (!m_elements) {
            _THROW_OOM();
        }
    }
    vector(std::initializer_list<_Ty> initializer)
        : vector(_VECTOR_INITIAL_SIZE) {
        for (auto element = std::begin(initializer);
            element < std::end(initializer);
            ++element) {
            push_back(*element);
        }
    }
    explicit vector(size_t _Count)
        : m_total_allocated_nodes(_Count)
    {
        m_elements = static_cast<_Ty*>(_STD calloc(_Count, sizeof(_Ty)));
        if (!m_elements) {
            _THROW_OOM();
        }
    }

    ~vector() {
        // RtlValidateHeap fails here, memory is corrupted sometime before this point.
        _STD free(m_elements);
    }

    void push_back(_Ty&& _Value) {
        if (!m_cache.empty()) {
            // take the position from the back of the stack
            auto _Position = m_cache.pop();
            auto* _Location = (m_elements + _Position);
            *_Location = _STD move(_Value);
            _Increment();
            return;
        }
        _Ensure_Capacity();
        auto* _Location = (m_elements + m_element_count);
        *_Location = _STD move(_Value);
        _Increment();
    }
    void push_back(_Ty const& _Value) {
        if (!m_cache.empty()) {
            // take the position from the back of the stack
            auto _Position = m_cache.pop();
            auto* _Location = (m_elements + _Position);
            *_Location = _Value;
            _Increment();
            return;
        }
        _Ensure_Capacity();
        auto* _Location = (m_elements + m_element_count);
        *_Location = _Value;
        _Increment();
    }

    _Ty&& remove(const size_t& _Position) {
        if (_Position > m_element_count)
            return _STD move(_Ty());
        if (m_cache.at_capacity()) {
            /* shifting the array down would invalidate the cache...*/
            throw std::exception("fuck that");
        }

        auto _Result = m_cache.push(_Position);
        if (_Result != Ok) {
            throw std::exception("not enough space to save position");
        }
        return _STD move(this->at_mut(_Position));
    }

    const _Ty& at(size_t _Pos) const {
        if (_Pos < 0 || _Pos > m_element_count) {
            return m_default;
        }
        return m_elements[_Pos];
    }
    _Ty& at_mut(size_t _Pos) {
        if (_Pos < 0 || _Pos > m_element_count) {
            return m_default;
        }
        return m_elements[_Pos];
    }

    void _setdef(_Ty&& _Default) noexcept {
        this->m_default = _STD move(_Default);
    }

    size_t size() const noexcept {
        return m_element_count;
    }
    constexpr size_t capacity() const noexcept {
        return m_total_allocated_nodes;
    }

    std::vector<_Ty> migrate() const noexcept {
        std::vector<_Ty> _Vec = {};
        for (auto i = 0; i < m_element_count; ++i) {
            _Vec.push_back(this->at(i));
        }
        return _Vec;
    }

    // iterator
    class iterator {
    private:
        _Ty* m_iterator{ nullptr };
    public:
        iterator(_Ty* _Position)
            : m_iterator(_Position)
        {}

        using difference_type = long;
        using value_type = long;
        using pointer = const long*;
        using reference = const long&;
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
            return m_iterator < other.m_iterator;
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

        _Ty& operator*() { return *m_iterator; }
    };

    iterator begin() noexcept { return iterator(m_elements); }
    iterator end() noexcept { return iterator(m_elements + size()); }
private:
    inline void _Ensure_Capacity() {
        /*check if adding another element would make this situation volitile*/
        if (!((m_element_count + 1) >= m_total_allocated_nodes)) {
            /*no problem*/
            return;
        }

        /*lets expand by (count + (count / 2)) (expand by half)*/
        auto _Total = m_total_allocated_nodes;
        auto _Newtotal = _Total + (_Total / 2);
        auto* _Newspace 
            = static_cast<_Ty*>(_STD calloc(_Newtotal, sizeof (_Ty)));
        if (!_Newspace) {
            _THROW_OOM();
        }
        /* copy the current nodes into our newly allocated space*/
        _STD memmove(_Newspace, m_elements, _Total);

        /* free the old space */
        _STD free(m_elements);

        /* re-assign m_elements */
        m_elements = _Newspace;

        /* update total capacity */
        m_total_allocated_nodes = _Newtotal;
    }
    inline void _Increment() {
        m_element_count++;
    }
};

template<typename _Ty>
using predicate = std::function<bool(_Ty const&)>;

// TODO: implement for util::vector
template<typename _Ty>
bool any(std::vector<_Ty> const& vec, predicate<_Ty> pred) {
    for (const auto& _Element : pred) {
        if (pred(_Element)) {
            return true;
        }
    }
    return false;
}

template<typename _Ty>
std::vector<_Ty> filter(std::vector<_Ty> const& vec, predicate<_Ty> pred) {
    std::vector<_Ty> _Vec = {};
    for (const auto& _Element : vec) {
        if (pred(_Element)) {
            _Vec.push_back(_Element);
        }
    }
    return _Vec;
}

template<typename _Ty>
using mutator = std::function<_Ty(_Ty const&)>;

template<typename _Ty>
vector<_Ty> map(vector<_Ty>& original, mutator<_Ty> mapper) {
    vector<_Ty> _Vec = {};
    for (const auto& _Element : original) {
        auto mutated = mapper(_Element);
        _Vec.push_back(mutated);
    }
    return _Vec;
}

template<typename _Ty>
std::vector<_Ty> map(std::vector<_Ty> const& original, mutator<_Ty> mapper) {
    std::vector<_Ty> _Vec = {};
    for (const auto& _Element : original) {
        auto mutated = mapper(_Element);
        _Vec.push_back(mutated);
    }
    return _Vec;
}

void display(auto& iterable) {
    std::cout << "[";
    auto end = std::end(iterable);
    for (auto start = std::begin(iterable);
        start < end;
        ++start) {
        if ((start + 1) < end) {
            std::cout << *start << ", ";
        }
        else
            std::cout << *start << "]";
    }
}

_UTIL_API_END

namespace std {
    template<typename _Ty>
    util::vector<_Ty>::iterator begin(util::vector<_Ty>& vec) {
        return vec.begin();
    }

    template<typename _Ty>
    util::vector<_Ty>::iterator end(util::vector<_Ty>& vec) {
        return vec.end();
    }
}

#include "dict.hpp"
