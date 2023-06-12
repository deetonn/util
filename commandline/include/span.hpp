#pragma once

#include "common.h"

_UTIL_API

// generic span
template<typename T>
class span {
private:
    T* start;
    T* end;
public:
    FTD_CONSTEXPR span(iterator<T> begin, iterator<T> end) noexcept {
        auto& f = *begin;
        auto& b = *end;

        this->start = &f;
        this->end = &b;
    }
    template<class iter>
    FTD_CONSTEXPR span(iter& iterable) noexcept
        : span(iterable.begin(), iterable.end())
    {}

    FTD_CONSTEXPR T& front() noexcept { return *start; }
    FTD_CONSTEXPR T& back() noexcept  { return *end; }

    FTD_CONSTEXPR size_t count() const noexcept {
        const auto _First = reinterpret_cast<size_t>(start);
        const auto _Sec = reinterpret_cast<size_t>(end);
        const auto _Diff = _Sec - _First;
        return _Diff / sizeof(T);
    }
};

// string_view
template<>
class span<char> {

};

_UTIL_API_END