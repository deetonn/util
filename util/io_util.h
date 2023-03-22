#pragma once

#include "common.h"
//#include "windows.h"

//#include "memory_util.h"

//#include "string.hpp"

#include <cstdio>

#include <format>

_UTIL_API

typedef struct _Io_stream {
    FILE* handle; /* The handle to the out stream */
    size_t calls; /* The amount of times this has been used to output data */
} _Iostream, io;

inline static _Iostream _Io_Streams[4] = {
    { stdout, 0 },
    { stderr, 0 },
    { stdin,  0 },
    { NULL,   0 }
};

#define STDOUT 0
#define STDERR 1
#define STDIN  2
#define STD_UNKNOWN 3

auto __io_func(unsigned mode) -> _Iostream* {
    if (mode != STDOUT && mode != STDERR && mode != STDIN)
        return &_Io_Streams[STD_UNKNOWN];
    return &_Io_Streams[mode];
}

static inline _Iostream* out = __io_func(STDOUT);
static inline _Iostream* err = __io_func(STDERR);
static inline _Iostream* in  = __io_func(STDIN);

auto tfprintf(_Io_stream* _Stream, const char* _Fmt, ...) -> size_t
{
    if (!_Stream)
        return 0;
    va_list args = {};
    __crt_va_start(args, _Fmt);
    auto count = vfprintf(_Stream->handle, _Fmt, args);
    __crt_va_end(args);
    _Stream->calls++;
    return count;
}

auto tprintf(const char* _Fmt, ...) -> size_t {
    va_list args = {};
    __crt_va_start(args, _Fmt);
    auto count = vfprintf(out->handle, _Fmt, args);
    __crt_va_end(args);
    out->calls++;
    return count;
}

auto ttl(_Io_stream* _Stream) -> size_t {
    if (_Stream)
        return _Stream->calls;
    return 0;
}

template<typename... Types>
auto writeln(
    const std::_Fmt_string<Types...> _Fmt,
    Types... _Args) -> void
{
    auto _Formatted = _STD vformat(_Fmt._Str, _STD make_format_args(_Args...));
    std::cout << _Formatted << "\n";
}

template<typename... Types>
auto write(
    const std::_Fmt_string<Types...> _Fmt,
    Types... _Args) -> void 
{
    auto _Formatted = _STD vformat(_Fmt._Str, _STD make_format_args(_Args...));
    std::cout << _Formatted;
}

template<typename... Types>
auto fmt(
    const std::_Fmt_string<Types...> _Fmt,
    Types... _Args) -> std::string {
    auto _Formatted = _STD vformat(_Fmt._Str, _STD make_format_args(_Args...));
    return _Formatted;
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