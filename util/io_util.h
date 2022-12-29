#pragma once

#include "common.h"

#include <cstdio>

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

_Iostream* __io_func(unsigned mode) {
    if (mode != STDOUT && mode != STDERR && mode != STDIN)
        return &_Io_Streams[STD_UNKNOWN];
    return &_Io_Streams[mode];
}

static inline _Iostream* out = __io_func(STDOUT);
static inline _Iostream* err = __io_func(STDERR);
static inline _Iostream* in  = __io_func(STDIN);

size_t tfprintf(_Io_stream* _Stream, const char* _Fmt, ...)
{
    if (!_Stream)
        return 0;
    va_list args = {};
    __crt_va_start(args, _Fmt);
    auto count = vfprintf(_Stream->handle, _Fmt, args);
    __crt_va_end(args);
    _Stream->handle++;
    return count;
}

size_t tprintf(const char* _Fmt, ...) {
    va_list args = {};
    __crt_va_start(args, _Fmt);
    auto count = vfprintf(out->handle, _Fmt, args);
    __crt_va_end(args);
    out->calls++;
    return count;
}

size_t ttl(_Io_stream* _Stream) {
    if (_Stream)
        return _Stream->calls;
    return 0;
}

_UTIL_API_END