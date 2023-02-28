#pragma once

#include "common.h"

_UTIL_API

template<typename T>
class optional {
private:
    T* m_value{ 0 };
public:
    constexpr optional() = default;
    constexpr optional(const T& _Init) noexcept {
        m_value = new (std::nothrow) T (_Init);
    }

    constexpr ~optional() noexcept {
        if (m_value) {
            delete m_value;
        }
    }

    constexpr BOOL has_value() const noexcept {
        return m_value != nullptr;
    }
    constexpr T unwrap() {
        if (!has_value())
            throw std::exception("optional contains no value");
        return *m_value;
    }
    constexpr T& unwrap_ref() {
        if (!has_value())
            throw std::exception("optional contains no value");
        return *m_value;
    }

    constexpr void swap(optional& other) noexcept {
        T* _Val = this->m_value;
        this->m_value = other.m_value;
        other.m_value = _Val;
    }
};

template<class T>
auto unwrap_optional(optional<T>& opt) -> T {
    return opt.unwrap();
}

template<typename T>
auto unwrap_or_panic(optional<T> _Opt) -> T {
    if (_Opt.has_value()) {
        return _Opt.unwrap();
    }
    panic("failed to unwrap optional");
}

_UTIL_API_END