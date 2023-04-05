#pragma once

#include "common.h"

_UTIL_API

template<typename T, size_t N>
class array {
public:
    using element = T;
    using value_type = element;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using iterator = _UTL iterator<element>;
    using size_type = size_t;
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

    FTD_CONSTEXPR pointer data() noexcept {
        return reinterpret_cast<element*>(elements);
    }

    FTD_CONSTEXPR reference at(size_t _Off) noexcept {
#if defined (_DEBUG)
        if (_Off > N || 0 > _Off) {
            panic("array sub-script out of range");
        }
#endif
        return elements[_Off];
    }

    FTD_CONSTEXPR const_reference at(size_t _Off) const noexcept {
        return elements[_Off];
    }
    FTD_CONSTEXPR _NODISCARD bool empty() const noexcept {
        // array is always initialized
        return false;
    }

    FTD_CONSTEXPR reference front() noexcept {
        return elements[0];
    }
    FTD_CONSTEXPR reference back() noexcept {
        return elements[size() - 1];
    }

    FTD_CONSTEXPR _NODISCARD size_t capacity() const noexcept {
        return N;
    }
    FTD_CONSTEXPR _NODISCARD size_t max_size() const noexcept {
        return capacity();
    }
    FTD_CONSTEXPR _NODISCARD size_t size() const noexcept {
        return N;
    }

    FTD_CONSTEXPR reference operator[](size_t _Off) noexcept {
        return _SELF at(_Off);
    }
    FTD_CONSTEXPR const_reference view(size_t _Off) const noexcept {
        return elements[_Off];
    }

    FTD_CONSTEXPR void fill(const_reference _Filler) noexcept {
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