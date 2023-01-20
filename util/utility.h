#pragma once

#ifndef _UTL
#define _UTL ::util::
#endif
#ifndef _NORETURN
#define _NORETURN [[noreturn]]
#endif

#include "type_info.h"
#include "string_util.h"
#include "memory_util.h"
#include "io_util.h"
#include "integral_util.h"
#include "collection_util.h"
#include "traits.h"
#include "error.h"
#include "types.h"
#include "meta.h"

#include "common.h"

#include <thread>
#include <stacktrace>

#include <iostream>
#include <vector>

#include <algorithm>
#include <format>

#if !defined(__linux__)
#include <windows.h>
#else
#include <unistd.h>
#endif

#include "hex_conversion.h"

_UTIL_API

using namespace std::chrono_literals;

_CONSTEXPR double __version() {
    return 0.1;
}

/* 
This function is templated to avoid it being compiled for
applications that dont use it.
*/
template<typename _Void = std::void_t<void>>
[[noreturn]] auto panic(const char* message) -> _Void {
    auto _Ostr = _UTL __io_func(STDERR);
    __assume(_Ostr != nullptr);
    __assume(message != nullptr);
    /*
    verify at least one byte has been written for
    proper io diagnostics
    */
    if (!_Ostr->calls) {
        _UTL tfprintf(_Ostr, "\n\0");
    }

    auto stack = _STD stacktrace::current();
    auto _Position = 0;
    // Traverse the stack trace backwards
    for (
        auto frame = stack.rbegin();
        frame != stack.rend();
        ++frame, _Position++
    ) 
    {
        const auto& _Fr = *frame;
        if (_Position == stack.size() - 2) {
            _UTL tfprintf(_Ostr,
                "%s <-- [panic here at line %zu in file %s]\n",
                _Fr.description().c_str(),
                _Fr.source_line(),
                _Fr.source_file().c_str());
        }
        else {
            _UTL tfprintf(_Ostr,
                "[%zu] %s\n",
                _Fr.source_line(),
                _Fr.description().c_str());
        }
    }

    _UTL tfprintf(
        _Ostr,
        "\n[panic]: %s\n", message
    );

    std::unreachable();
}

/*
This function is templated to avoid it being compiled for
applications that dont use it.
This function will panic if the condition is true.
*/
template<typename _Void = std::void_t<void>>
auto panic_if(bool _Cond, const char* _Msg) -> _Void {
    if (_Cond)
        _UTL panic(_Msg);
}

template<typename _Void = std::void_t<void>>
auto panic_if_not(bool _Cond, const char* _Msg) -> _Void {
    if (!_Cond)
        _UTL panic(_Msg);
}

_NORETURN auto quit() -> decltype(auto) {
    _UTL panic("quit() was called");
}

template<typename U>
inline U declval() {}

_UTIL_API_END