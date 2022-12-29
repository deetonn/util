#pragma once

#include "common.h"
#include "memory_util.h"

#include <concepts>

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
    explicit vector(size_t _Count)
        : m_total_allocated_nodes(_Count)
    {
        m_elements = static_cast<_Ty*>(_STD calloc(_Count, sizeof(_Ty)));
        if (!m_elements) {
            _THROW_OOM();
        }
    }

    ~vector() {
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

_UTIL_API_END

#include "dict.hpp"
