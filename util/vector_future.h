#pragma once

#include "common.h"

#include "memory_util.h"

#ifndef _UTL
#define _UTL ::utl::
#endif

#ifndef _SELF
#define _SELF this->
#endif

_UTIL_EXPERIMENTAL

template<typename T, typename A = _UTL default_allocator>
class vector {
public:
    using value_type = T;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using size_type = size_t;
    using self = vector<T, A>;
    using difference = unsigned int;

    using iterator = iterator<T>;

private:
    T* _M_Elems      { nullptr };
    A  _M_Al         {};
    size_type _M_Cap { 0 };
    size_type _M_Siz { 0 };

public:
    vector() {}
    inline vector(std::initializer_list<value_type> _Init) {
        for (const auto& _Initializer : _Init) {
            _SELF push_back(_Initializer);
        }
    }
    vector(self const&) = default;
    vector(self&&) = default;

    size_type push_back(const_reference _Val) noexcept {
        _SELF _Maybe_expand();
        auto _Pos = _SELF _M_Siz++;
        _SELF _M_Elems[_Pos] = _Val;
        return _Pos;
    }
    size_type push_back(value_type&& _Val) noexcept {
        _SELF _Maybe_expand();
        auto _Pos = _SELF _M_Siz++;
        _SELF _M_Elems[_Pos] = _STD move(_Val);
        return _Pos;
    }

    template<class... Args>
    size_type emplace_back(Args&&... args) noexcept {
        auto object = T(_STD forward<Args>(args)...);
        return this->push_back(_STD move(object));
    }

    const T& at(size_type _Off) const noexcept {
        return _M_Elems[_Off];
    }
    T& at(size_type _Off) noexcept {
        return _M_Elems[_Off];
    }

    // getters
    size_type size() const noexcept {
        return _SELF _M_Siz;
    }
    A& allocator() noexcept {
        return _SELF _M_Al;
    }

    // iterator based functions
    typename self::iterator begin() {
        return iterator{ _M_Elems, size() };
    }
    typename self::iterator end() {
        auto _Sz = size();
        return iterator{ _M_Elems + _Sz, _Sz };
    }

private:
    void _Maybe_expand() noexcept {
        auto _Count = this->_M_Siz;
        auto _Cap   = this->_M_Cap;
        size_type _Diff = _Cap - _Count;

        if (_Diff && _Diff >= 1) {
            // OK!
            return;
        }

        A& _Al = this->allocator();

        if (_Cap == 0) {
            // First allocation, simply allocate one element
            this->_M_Elems = _Al.alloc<T>();
            this->_M_Cap = 1;
            return;
        }

        size_type _Newsize = this->_M_Cap * 2;
        T* _New_elems = _Al.alloc_many<T>(_Newsize);
        _UTL memcpy(_New_elems, this->_M_Elems, this->_M_Siz * sizeof(T));

        // on 3 elements, this fails...
        _Al.destroy(this->_M_Elems);

        this->_M_Elems = _New_elems;
        this->_M_Cap = _Newsize;
    }
};

_UTIL_API_END