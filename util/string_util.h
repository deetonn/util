#pragma once

#include "common.h"
#include "error.h"

#include <format>

_UTIL_API

inline size_t strlen(const char* _Str, BOOL _CountTerminator = false) {
    size_t _Result = 0;
    while (*(_Str++))
        _Result++;
    return _CountTerminator ? _Result + 1 : _Result;
}

inline void* memcpy(const uint8_t* _Src, uint8_t* _Dst, size_t _Count) {
    for (auto i = 0; i < _Count; ++i) {
        _Dst[i] = _Src[i];
    }

    return _Dst;
}

inline void* memcpy(const void* _Src, void* _Dst, size_t _Count)
{
    const auto* _CSrc = reinterpret_cast<const uint8_t*>(_Src);
    auto*       _CDst = reinterpret_cast<uint8_t*>(_Dst);

    return memcpy(_CSrc, _CDst, _Count);
}

_UTIL_API_END

using namespace std::string_literals;
using namespace std::chrono_literals;