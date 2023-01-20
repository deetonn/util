#pragma once

#include "common.h"
#include "error.h"

#include <format>

_UTIL_API

inline size_t strlen(const char* _Str, bool _CountTerminator = false) {
    size_t _Result = 0;
    while (*(_Str++))
        _Result++;
    return _CountTerminator ? _Result + 1 : _Result;
}

inline void* memcpy(const uint8_t* _Src, uint8_t* _Dst, size_t _Count) {
    for (auto i = 0; i < _Count; ++i) {
        _Dst[i] = _Src[i];
    }

    return _Dst;
}

inline void* memcpy(const void* _Src, void* _Dst, size_t _Count)
{
    const auto* _CSrc = reinterpret_cast<const uint8_t*>(_Src);
    auto*       _CDst = reinterpret_cast<uint8_t*>(_Dst);

    return memcpy(_CSrc, _CDst, _Count);
}

enum class LanguageCode {
    EN_US,
    EN_EU
};

typedef struct _Svs_Provider_t {
    std::string root_domain;
    std::string endpoint;

    int bytes_received;
    int bytes_sent;

    std::string content_type;
} ServiceProvider, *PServiceProvider;

std::string provider_to_string(ServiceProvider const* provider)
{
    return std::format(
        "({}{}): [{} received; {} sent; content-type: `{}`]",
        provider->root_domain,
        provider->endpoint,
        provider->bytes_received,
        provider->bytes_sent,
        provider->content_type);
}

inline ErrorOr<std::string> translate(
    std::string const& original, 
    LanguageCode const& code,
    ServiceProvider* service = nullptr)
{
    if (service) {
        service->bytes_received = 0;
        service->bytes_sent = 0;
        service->content_type = "content/json";
    }
    return Error("not yet implemented");
}

namespace typedefs {
    using string = std::string;
    using c_string = const char*;
}

_UTIL_API_END

using namespace std::string_literals;
using namespace std::chrono_literals;