#pragma once

#ifndef _UTIL_API
#define _UTIL_API namespace utl {
#endif

#ifndef _UTIL_API_END
#define _UTIL_API_END }
#endif

#ifndef _UTIL_EXPERIMENTAL
#define _UTIL_EXPERIMENTAL namespace utl::future {
#endif

#ifndef _W
#define _W(X) L##X
#endif

#ifndef _IMPLICIT
#define _IMPLICIT
#endif

#ifndef FTD_CONSTEXPR
#define FTD_CONSTEXPR inline constexpr
#endif

#ifndef FTD_NODISCARD
#define FTD_NODISCARD [[nodiscard]]
#endif

#include <string>
#include <format>

#include <iostream>

#include "io_util.h"
#include "types.h"

#include <stacktrace>

#ifndef FTD_NORETURN
#define _NORETURN [[noreturn]]
#endif

_UTIL_API

/*
* All panic functions are templated to avoid them being compiled
* if they are not used.
*/

template<typename _Void = std::void_t<void>>
FTD_NORETURN constexpr inline auto panic(const char* message) -> _Void {
    if (std::is_constant_evaluated()) {
        // constant eval context requires exceptions
        throw std::exception(message);
    }

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

template<class... Types>
FTD_NORETURN constexpr inline auto vpanic(
    const std::_Fmt_string<Types...> _Fmt,
    Types... Args) 
{
    auto fmt = std::format(_Fmt, Args...);
    panic(fmt.c_str());
}

template<typename _Void = std::void_t<void>>
FTD_NORETURN constexpr inline auto panic_if(BOOL _Cond, const char* _Msg) -> _Void {
    if (_Cond)
        _UTL panic<_Void>(_Msg);
}

template<typename _Void = std::void_t<void, size_t>>
FTD_NORETURN constexpr inline auto panic_if_not(BOOL _Cond, const char* _Msg) -> _Void {
    if (!_Cond)
        _UTL panic<_Void>(_Msg);
}

FTD_NORETURN constexpr auto quit() -> decltype(auto) {
    _UTL panic("quit() was called");
}

_UTIL_API_END