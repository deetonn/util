#pragma once

#include "type_info.h"
#include "string_util.h"
#include "memory_util.h"
#include "io_util.h"
#include "integral_util.h"
#include "collection_util.h"
#include "error.h"

#include "common.h"

_UTIL_API

enum : int { release, debug, dev };
#define nameof(X) #X

typedef struct {
    int major, minor;
    int mode;
    const char* mode_str;
} version;

constexpr version __version = {
    1, 1, dev, nameof(dev)
};

std::string version_to_string() {
    return std::format("{}.{}-{}",
        __version.major, __version.minor, __version.mode_str);
}

_UTIL_API_END