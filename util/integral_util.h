#pragma once

#include "common.h"

_UTIL_API

#if defined(_MSC_VER)
#if (_M_IX86)
#define X86
#else
#define X64
#endif
#endif

#if defined(GCC)
#if defined (__i386__)
#define X86
#else
#define X64
#endif
#endif

#if defined (X64)
using size_t = unsigned long long int;
#else
using size_t = unsigned int;
#endif

#if defined (__cplusplus)
#define NULL 0
#else
#define NULL ((void*) 0)
#endif

template<size_t _Min, size_t _Max>
class between {
private:
    size_t m_number{ _Max - _Min };
public:
    _IMPLICIT constexpr between(size_t _Initializer) {
        if constexpr(_Initializer > _Max || _Min > _Initializer) {
            // keep throw, constexpr context
            throw std::exception("out-of-range initializer");
        }
        m_number = _Initializer;
    }

    size_t set(size_t s) {
        if (s > _Max || _Min > s) {
            util::panic("value out of range to set between<>");
        }
        m_number = s;
        return m_number;
    }

    constexpr size_t value() const noexcept {
        return m_number;
    }
};

_UTIL_API_END

#include "limits.h"

/* using namespace util::typedefs; instead of util::* or using namespace util; */
namespace util::typedefs {
    using i8 = char;
    using u8 = unsigned char;

    using i16 = short;
    using u16 = unsigned short;

    using i32 = int;
    using u32 = unsigned;

    using i64 = long long;
    using u64 = unsigned long long int;
}