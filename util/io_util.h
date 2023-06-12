#pragma once

#include "common.h"
//#include "windows.h"

//#include "memory_util.h"

//#include "string.hpp"

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

/*  
* Actual filesystem related tasks rather than just 
* I/O stream & formatting related tasks.
* 
* (Windows Only)
*/

/* helper for getting windows temporary path */

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

//inline static string temporary_path() {
//    static safe_ptr<string> data = {};
//
//    if (data->is_empty()) {
//        char buffer[MAX_PATH] = {};
//        auto result = GetTempPathA(MAX_PATH, buffer);
//
//        if (!result)
//            panic("failed to load temporary directory!");
//
//        data->append(buffer);
//    }
//
//    return *data;
//}

//class temp_file {
//private:
//    FILE* m_handle;
//    string m_name;
//public:
//    temp_file() {
//
//    }
//};

_UTIL_API_END