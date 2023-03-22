#pragma once

#include "common.h"

#ifndef FTD_ITERATORS
#define FTD_ITERATORS

_UTIL_API

template<typename T>
class iterator {
private:
    T* _Elems{ nullptr };
    size_t _Pos{ 0 };
public:
    iterator(T* _Elems, size_t const& _Size)
        : _Elems(_Elems), _Pos(_Size)
    {}

    using difference_type = uintptr_t;
    using value_type = T;
    using pointer = T*;
    using reference = T&;
    using iterator_category = std::forward_iterator_tag;

    iterator<T>& operator ++() {
        _Elems++, _Pos++;
        return *this;
    }
    iterator<T> operator ++(int) {
        iterator self = *this;
        ++(*this);
        return self;
    }

    BOOL operator <(iterator<T> const& other) {
        return _Pos <= other._Pos;
    }
    BOOL operator >(iterator<T> const& other) {
        return !(*this < other);
    }

    iterator operator +(size_t number) {
        return { _Elems, _Pos + number };
    }

    //auto operator <=>(iterator const&) = default;

    BOOL operator==(iterator<T> other) const { return this->_Elems == other._Elems; }
    BOOL operator!=(iterator<T> other) const { return !(*this == other); }

    reference operator*() {
        return *_Elems;
    }

    static inline iterator create(T* _Elems, size_t const& _Size) {
        return iterator(_Elems, _Size);
    }
};

template<typename T>
class const_iterator {
private:
    T* _Elems{ nullptr };
    size_t _Pos{ 0 };
public:
    const_iterator(const T* _Elems, size_t _Size)
        : _Elems(_Elems), _Pos(_Size)
    {}

    using difference_type = uintptr_t;
    using value_type = T;
    using pointer = const T*;
    using reference = const T&;
    using iterator_category = std::forward_iterator_tag;

    const_iterator<T>& operator ++() {
        _Elems++, _Pos++;
        return *this;
    }
    const_iterator<T> operator ++(int) {
        const_iterator self = *this;
        ++(*this);
        return self;
    }

    BOOL operator <(const_iterator<T> const& other) {
        return _Pos <= other._Pos;
    }
    BOOL operator >(const_iterator<T> const& other) {
        return !(*this < other);
    }

    const_iterator operator +(size_t number) {
        return { _Elems, _Pos + number };
    }

    //auto operator <=>(iterator const&) = default;

    BOOL operator==(const_iterator<T> other) const { return this->_Elems == other._Elems; }
    BOOL operator!=(const_iterator<T> other) const { return !(*this == other); }

    reference operator*() {
        return *_Elems;
    }

    static inline const_iterator create(T* _Elems, size_t const& _Size) {
        return const_iterator(_Elems, _Size);
    }
};

template<typename T, size_t N>
class constexpr_iterator {
private:
    const T values[N];
    size_t _Pos{ 0 };
public:
    FTD_CONSTEXPR constexpr_iterator(const T Vals[N]) noexcept {
        values = Vals;
    }

    FTD_CONSTEXPR const T& at(size_t offset) const noexcept {
        if constexpr (offset > N) {
            throw std::exception("out of range access");
        }

        return values[offset];
    }

    FTD_CONSTEXPR constexpr_iterator operator ++() {
        constexpr_iterator it = { values };
        it._Pos = _Pos + 1;
        return it;
    }
    FTD_CONSTEXPR constexpr_iterator operator ++(int) {
        constexpr_iterator it = { values };
        it._Pos = _Pos + 1;
        return it;
    }

    FTD_CONSTEXPR BOOL operator <(constexpr_iterator const& other) {
        return _Pos <= other._Pos;
    }
    FTD_CONSTEXPR BOOL operator >(constexpr_iterator const& other) {
        return !(*this < other);
    }

    FTD_CONSTEXPR constexpr_iterator operator +(size_t number) {
        return { values, _Pos + number };
    }

    //auto operator <=>(iterator const&) = default;

    FTD_CONSTEXPR BOOL operator==(constexpr_iterator other) const { return this->_Elems == other._Elems; }
    FTD_CONSTEXPR BOOL operator!=(constexpr_iterator other) const { return !(*this == other); }

    FTD_CONSTEXPR const T& operator*() const noexcept {
        return values[_Pos];
    }

    FTD_CONSTEXPR static constexpr_iterator create(const T(&Elems)[N]) {
        return constexpr_iterator(Elems);
    }
};

#endif

_UTIL_API_END