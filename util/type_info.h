#pragma once

#include <typeinfo>

#include "common.h"

_UTIL_API

template<typename _Ty>
class type_info {
public:
    static inline decltype(typeid(_Ty).name()) name() {
        auto& info = typeid(_Ty);
        return info.name();
    }

    static inline decltype(typeid(_Ty).raw_name()) raw_name() {
        auto& info = typeid(_Ty);
        return info.raw_name();
    }

    static inline decltype(typeid(_Ty).hash_code()) hash_code() {
        auto& info = typeid(_Ty);
        return info.hash_code();
    }
};

template<typename _Ty>
inline const char* nameof() {
    return type_info<_Ty>::name();
}

template<typename _Ty>
inline const char* raw_nameof() {
    return type_info<_Ty>::raw_name();
}

template<typename _Ty>
inline size_t hashcode_of() {
    return type_info<_Ty>::hash_code();
}

template<class _Ty>
constexpr size_t size() noexcept {
    return sizeof(_Ty);
}

_UTIL_API_END