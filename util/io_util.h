#pragma once

#include "common.h"

#include <cstdio>

#include <format>

_UTIL_API

template<typename... Types>
auto writeln(
    const std::format_string<Types...> _Fmt,
    Types... _Args) -> void
{
    auto _Formatted = _STD vformat(_Fmt.get(), _STD make_format_args(_Args...));
    std::cout << _Formatted << "\n";
}

template<typename... Types>
auto write(
    const std::format_string<Types...> _Fmt,
    Types... _Args) -> void 
{
    auto _Formatted = _STD vformat(_Fmt.get(), _STD make_format_args(_Args...));
    std::cout << _Formatted;
}

_UTIL_API_END