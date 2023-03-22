#pragma once

#include "common.h"

_UTIL_API

template<typename T, size_t N>
class array {
public:
    using element = T;
    using iterator = _UTL iterator<element>;
    using self = array<T, N>;
private:
    element elements[N]{};
public:
    FTD_CONSTEXPR array() = default;
    FTD_CONSTEXPR array(const array&) = default;
    FTD_CONSTEXPR array(array&&) = default;

    FTD_CONSTEXPR array(std::initializer_list<element> _Init) {
        if (FTD_CONSTEVAL && _Init.size() > N) {
            // for consteval 
            throw std::exception("initializer is too large");
        }
        else if (_Init.size() > N) {
            _UTL vpanic("array initializer out of bounds! ({} > {})", _Init.size(), N);
        }

        size_t pos = 0;
        for (const auto& element : _Init) {
            this->elements[pos] = element;
            ++pos;
        }
    }

    FTD_CONSTEXPR element* data() noexcept {
        return reinterpret_cast<element*>(elements);
    }

    FTD_CONSTEXPR element& at(size_t _Off) noexcept {
#if defined (_DEBUG)
        if (_Off > N || 0 > _Off) {
            panic("array sub-script out of range");
        }
#endif
        return elements[_Off];
    }
    FTD_CONSTEXPR element& front() noexcept {
        return elements[0];
    }
    FTD_CONSTEXPR element& back() noexcept {
        return elements[size() - 1];
    }

    FTD_CONSTEXPR size_t capacity() const noexcept {
        return N;
    }
    FTD_CONSTEXPR size_t size() const noexcept {
        return N;
    }

    FTD_CONSTEXPR element& operator[](size_t _Off) noexcept {
        return _SELF at(_Off);
    }
    FTD_CONSTEXPR const element& view(size_t _Off) const noexcept {
        return (*this)[_Off];
    }

    FTD_CONSTEXPR void fill(const T& _Filler) noexcept {
        for (auto i = 0; i < size(); ++i) {
            this->elements[i] = _Filler;
        }
    }

    FTD_CONSTEXPR void swap(array& _Other) noexcept {
        auto _Copy = _Other.elements;
        _Other.elements = this->elements;
        this->elements = _Copy;
    }
    FTD_CONSTEXPR void swap(std::unique_ptr<array>& _Other) {
        this->swap(*_Other);
    }
    FTD_CONSTEXPR void swap(_UTL safe_ptr<array>& _Other) {
        swap(*_Other);
    }

    FTD_CONSTEXPR iterator begin() noexcept {
        return iterator(
            reinterpret_cast<T*>(&elements),
            0
        );
    }
    FTD_CONSTEXPR iterator end() noexcept {
        return iterator(
            reinterpret_cast<T*>(&elements[N]),
            capacity()
        );
    }

    FTD_CONSTEXPR auto _raw() noexcept -> decltype(elements)* {
        return &elements;
    }
};

_UTIL_API_END