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
    element elements[N];
public:
    FTD_CONSTEXPR array() = default;
    FTD_CONSTEXPR array(const array&) = default;
    FTD_CONSTEXPR array(array&&) = default;

    FTD_CONSTEXPR array(std::initializer_list<element> const& _Init) {
#if defined (_DEBUG)
        _STL_VERIFY(_Init.size() <= N, "initializer is out of range");
#endif
        size_t pos = 0;
        for (const element& elem : _Init) {
            this->elements[pos] = elem;
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
        return elements[_Off];
    }

    FTD_CONSTEXPR void fill(const T& _Filler) noexcept {
        for (auto i = 0; i < size(); ++i) {
            elements[i] = _Filler;
        }
    }
    FTD_CONSTEXPR BOOL contains(const T& _El) const noexcept {
        for (size_t s = 0; s < size(); ++s) {
            const element& el = this->elements[s];
            if (el == _El) {
                return true;
            }
        }
        return false;
    }

    FTD_CONSTEXPR void swap(array& _Other) noexcept {
        auto _Copy = _Other.elements;
        _Other.elements = this->elements;
        this->elements = _Copy;
    }
    FTD_CONSTEXPR void swap(std::unique_ptr<array>& _Other) noexcept {
        this->swap(*_Other);
    }
    FTD_CONSTEXPR void swap(_UTL safe_ptr<array>& _Other) noexcept {
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
            reinterpret_cast<T*>(&elements),
            capacity()
        );
    }

    FTD_CONSTEXPR auto _raw() noexcept -> decltype(elements)* {
        return &elements;
    }
};

template<typename It>
FTD_CONSTEXPR auto fill(
    It& iterator,
    const typename It::element& value
) noexcept {
    iterator.fill(value);
}

template<typename It>
FTD_CONSTEXPR auto next(
    It& iterator
) noexcept -> typename It::reference {
    iterator++;
    return *iterator;
}

template<typename It>
FTD_CONSTEXPR auto begin(
    It& iterator
) noexcept -> typename It::iterator {
    return iterator.begin();
}

template<typename It>
FTD_CONSTEXPR auto end(
    It& iterator
) noexcept -> typename It::iterator {
    return iterator.end();
}

template<typename It>
FTD_CONSTEXPR auto raw(It& iterator) 
noexcept -> decltype(iterator._raw()) {
    return iterator._raw();
}

_UTIL_API_END