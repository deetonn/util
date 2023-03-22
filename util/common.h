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

#ifndef _MyPtr
#define _MyPtr() *this
#endif

#ifndef _MyMutPtr
#define _MyMutPtr() const_cast<std::remove_const_t<decltype(*this)*>(this)
#endif

#include <string>
#include <format>

#include <iostream>

#include "io_util.h"
#include "types.h"
// #include <windows.h>

#ifndef FTD_ITERATORS
#include "iterators.hpp"
#endif

#include <stacktrace>

#ifndef FTD_NORETURN
#define _NORETURN [[noreturn]]
#endif

#ifndef FTD_CONSTEVAL
#define FTD_CONSTEVAL __builtin_is_constant_evaluated()
#endif

_UTIL_API

/*
* All panic functions are templated to avoid them being compiled
* if they are not used.
*/

template<typename _Void = std::void_t<void>>
FTD_NORETURN constexpr inline auto panic(const char* message) -> _Void {
    if (FTD_CONSTEVAL) {
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

    if constexpr (std::is_same<void, _Void>::value) {
        std::unreachable();
    }
    else {
        return _Void{};
    }
}

template<class... Types>
FTD_NORETURN constexpr inline auto vpanic(
    const std::_Fmt_string<Types...> _Fmt,
    Types&&... Args) 
{
    panic<void>(std::format<Types...>(_Fmt, _STD move(Args)...).c_str());
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

#define FTD_ASSERT(expr, message) if (!(expr)) { utl::vpanic("assertion failure! '{}' -- {}", #expr, message); }

#ifndef FTD_VERIFY
#define FTD_VERIFY(expr, message) FTD_ASSERT(expr, message)
#endif

const std::vector<std::string>& args()
noexcept {
    static std::vector<std::string> _Vec = {};
    if (_Vec.empty()) {
        auto& argc = *__p___argc();
        auto argv = *__p___argv();

        for (auto i = 0; i < argc; ++i) {
            auto s = std::string{ argv[i] };
            _Vec.push_back(_STD move(s));
        }
    }
    return _Vec;
}

size_t argc()
noexcept {
    return args().size();
}

constexpr auto path()
noexcept -> std::string const& {
    return args().front();
}

_UTIL_API_END

#ifndef FTD_SETTINGS_HPP
#define FTD_SETTINGS_HPP
#include "settings.hpp"
#endif