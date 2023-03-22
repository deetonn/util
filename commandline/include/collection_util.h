#pragma once

#include "common.h"
#include "memory_util.h"

#include <functional>

#include "traits.h"
#include "types.h"

#include "vector_future.h"

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

    BOOL at_capacity() const noexcept {
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

        BOOL operator==(iterator other) const { return this->m_iterator == other.m_iterator; }
        BOOL operator!=(iterator other) const { return !(*this == other); }

        _Ty& operator*() { return *m_iterator; }
    };

    iterator begin() noexcept { return iterator(m_stack); }
    iterator end() noexcept { return iterator(m_stack + m_position); }
    std::reverse_iterator<_Ty> rbegin() {
        return std::reverse_iterator<_Ty>(end());
    }
    std::reverse_iterator<_Ty> rend() {
        return std::reverse_iterator<_Ty>(begin());
    }
};

#ifndef _VECTOR_INITIAL_SIZE
#define _VECTOR_INITIAL_SIZE 8
#endif

#ifndef _VECTOR_CACHE_SIZE 
#define _VECTOR_CACHE_SIZE 10
#endif

#define _ERROR_OOM_() _UTL panic("failed to allocate enough memory");

template<collection_suitable _Ty, typename _Alloc = _UTL default_allocator>
class [[deprecated("use future::vector")]] vector {
private:
    _Ty* m_elements{ nullptr };
    size_t m_element_count{ 0 };
    size_t m_total_allocated_nodes{ 0 };

    stack<size_t, _VECTOR_CACHE_SIZE> m_cache{};

    _Ty m_default = {};
    _Alloc _Al{};
public:
    vector()
        : m_total_allocated_nodes(_VECTOR_INITIAL_SIZE)
    {
        m_elements = _Al.alloc_many<_Ty>(8);
        /*m_element_count = 0*/

        if (!m_elements) {
            _ERROR_OOM_();
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
            _ERROR_OOM_();
        }
    }

    ~vector() {
        // RtlValidateHeap fails here, memory is corrupted sometime before this point.
        _Al.destroy(this->m_elements);
    }

    void push_back(_Ty&& _Value) {
        if (!m_cache.empty()) {
            // take the position from the back of the stack
            auto _Position = m_cache.pop();
            auto* _Location = (m_elements + _Position);
            *_Location = _STD move(_Value);
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
            _UTL panic("cannot shift the array down while at capacity");
        }

        auto _Result = m_cache.push(_Position);
        if (_Result != Ok) {
            _UTL panic("not enough space to save position");
        }
        return _STD move(this->at_mut(_Position));
    }

    const _Ty& at(size_t _Pos) const {
        return this->at_mut(_Pos);
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

        using difference_type = uintptr_t;
        using value_type = _Ty;
        using pointer = const _Ty*;
        using reference = const _Ty&;
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

        BOOL operator <(iterator const& other) {
            return m_iterator < other.m_iterator;
        }
        BOOL operator >(iterator const& other) {
            return !(*this < other);
        }

        iterator operator +(size_t number) {
            return m_iterator + number;
        }

        //auto operator <=>(iterator const&) = default;

        BOOL operator==(iterator other) const { return this->m_iterator == other.m_iterator; }
        BOOL operator!=(iterator other) const { return !(*this == other); }

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
            _ERROR_OOM_();
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

void display(auto& iterable) {
    std::cout << "Vector(" << iterable.size() << ") [";
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

auto length(const auto& iterator) -> size_t {
    return iterator.size();
}

_UTIL_API_END

namespace std {
    template<typename _Ty>
    typename utl::future::vector<_Ty>::iterator begin(_UTL future::vector<_Ty>& vec) {
        return vec.begin();
    }

    template<typename _Ty>
    typename _UTL future::vector<_Ty>::iterator end(_UTL future::vector<_Ty>& vec) {
        return vec.end();
    }
}

// Misc containers

#include "dict.hpp"
#include "bits.hpp"