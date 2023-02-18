#pragma once

#ifndef _UTL
#define _UTL ::utl::
#endif
#ifndef _NORETURN
#define _NORETURN [[noreturn]]
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

_CONSTEXPR double __version() {
    return 0.1;
}

template<typename U>
inline U declval() {}

template<typename... Types>
auto print(
    const std::_Fmt_string<Types...> _Fmt,
    Types... _Args) -> void
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

_UTIL_API_END

namespace ftd = utl;
namespace future = ftd::future;

#endif