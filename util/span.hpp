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
    FTD_CONSTEXPR T& back() noexcept  { return *end;   }

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
private:
    const char* m_bytes;
    size_t m_length;
public:
    FTD_CONSTEXPR span(const std::string_view _Std_string) noexcept {
        m_bytes = _Std_string.data();
        m_length = _Std_string.size();
    }
    FTD_CONSTEXPR span(const char* _Builtin_string) noexcept {
        m_bytes = _Builtin_string;
        m_length = std::char_traits<char>::length(_Builtin_string);
    }

    FTD_CONSTEXPR const char* data() const noexcept {
        return m_bytes;
    }
    FTD_CONSTEXPR const char& at(size_t _Off) const noexcept {
        return m_bytes[_Off];
    }

    FTD_CONSTEXPR const char& front() const noexcept {
        return m_bytes[0];
    }
    FTD_CONSTEXPR const char& back() const noexcept {
        return m_bytes[m_length - 1];
    }

    FTD_CONSTEXPR size_t size() const noexcept {
        return m_length;
    }
};

using string_view = span<char>;

_UTIL_API_END