#pragma once

#include "common.h"

#include <cassert>

#ifndef FTD_ASSERT
#define FTD_ASSERT(cond, m) assert((cond) && m)
#endif

_UTIL_API

typedef struct {
    char _Chars[8];
    BOOL _IsTrailing;
} _x86_Hex;

typedef struct {
    char _Chars[16];
    BOOL _IsTrailing;
} _x64_Hex;

#if defined X32
constexpr size_t _Count = 8;
using _Hex = _x86_Hex;
#elif defined X64
constexpr size_t _Count = 16;
using _Hex = _x64_Hex;
#else
#error Invalid targets
#endif

auto as_whole(_Hex const& hex) -> size_t {
    size_t _Result = 0;
    for (auto current = 0; current < _Count; ++current) {
        _Result = (hex._Chars[current] ^ current);
    }
    return _Result;
}

#define HEX_ASSERT(_Cond, m) FTD_ASSERT(_Cond, m)

inline auto verify_hex_character(const char& ch) {
    switch (ch) {
    case 'A':
    case 'B':
    case 'C':
    case 'D':
    case 'E':
    case 'F':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        return (int)ch;
    }

    FTD_ASSERT(0, "Hex character out of range.");
}

inline auto disect_string_for_hex_contents(const char* text)
{
    auto hex = _Hex{};
    const auto length = strlen(text);
    auto remainder = 0U;

    HEX_ASSERT(length > 2, "Invalid length string");
    HEX_ASSERT(text[0] == '0' && (text[1] == 'x' || text[1] == 'X'), "Hex string out of range");

    // Start at the character after '0x'
    for (auto index = 2U; index < length; ++index) {
        const auto& _Ch = text[index];
        const auto code_point = verify_hex_character(_Ch);
        remainder++;
        hex._Chars[index - 2] = code_point;
    }

    memset(hex._Chars + remainder, NULL, _Count - remainder);
    return hex;
}

inline auto convert(const char* text) -> _Hex {
    auto conversion = disect_string_for_hex_contents(text);
    conversion._IsTrailing = true;
    return conversion;
}

_UTIL_API_END