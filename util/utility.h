#pragma once

#ifndef _UTL
#define _UTL ::utl::
#endif

#ifndef _FTD
#define _FTD ftd::
#endif

#ifndef FTD_NORETURN
#define FTD_NORETURN [[noreturn]]
#endif

#ifndef __FTD
#define __FTD

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
#include "defer.hpp"
#include "lazy.hpp"
#include "singleton.hpp"
#include "function.hpp"
#include "option.hpp"
#include "lexing.hpp"
#include "context.hpp"
#include "array.hpp"
#include "fs.hpp"

#include "common.h"

#include <thread>

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

FTD_CONSTEXPR double __version() {
    return 0.1;
}

template<typename U>
inline U declval() {}

template<typename... Types>
auto print(
    const std::format_string<Types...> _Fmt,
    Types&&... _Args) -> void
{
    _UTL writeln<Types...>(_Fmt, _Args...);
}

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

constexpr auto check_version() -> bool {
    constexpr auto baseline_lambda =
        [&]() {};
    constexpr auto basic_defer_size = 
        sizeof(singular_defer_context<decltype(baseline_lambda)>);

    if constexpr (basic_defer_size != 1) {
        throw std::exception("defer size mismatch!");
    }

    constexpr auto variadic_lambda
        = [&](size_t, size_t) {};

    constexpr auto variadic_defer_size =
        sizeof(singular_variadic_defer_context
            <decltype(variadic_lambda), int, int>);

    if constexpr (variadic_defer_size != 12) {
        throw std::exception("variadic defer size mismatch!");
    }

    return true;
}

#define FTD_CHECKVERSION() constexpr auto _$$Vcheck = ftd::check_version()

#define FTD_MAIN()                           \
        auto _Res = ftd_main(ftd::args());   \
        if (_Res.has_problem()) {            \
            ftd::panic(_Res.error().what()); \
        }                                    \
        return _Res.unwrap();

_UTIL_API_END

namespace ftd = utl;
namespace future = ftd::future;

#endif