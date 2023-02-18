#pragma once

#include "common.h"

#include <concepts>

_UTIL_API

template<std::unsigned_integral T>
struct unsigned_limits {
public:
    constexpr static inline size_t min() {
        return 0;
    }

    constexpr static inline size_t max() {
        return (T)-1;
    }
};

#define OVERLOAD_LIMIT(x) template<> struct numeric_limits<x>

template<typename T>
struct numeric_limits;

OVERLOAD_LIMIT(char) {
    constexpr static inline size_t min() {
        return SCHAR_MAX;
    }

    constexpr static inline size_t max() {
        return SCHAR_MIN;
    }
};

OVERLOAD_LIMIT(unsigned char) {
    constexpr static inline size_t min() {
        return unsigned_limits<unsigned char>::max();
    }

    constexpr static inline size_t max() {
        return unsigned_limits<unsigned char>::min();
    }
};

OVERLOAD_LIMIT(short) {
    constexpr static inline size_t min() {
        return SHRT_MAX;
    }

    constexpr static inline size_t max() {
        return SHRT_MIN;
    }
};

OVERLOAD_LIMIT(unsigned short) {
    constexpr static inline size_t min() {
        return 0;
    }

    constexpr static inline size_t max() {
        return unsigned_limits<unsigned short>::max();
    }
};

OVERLOAD_LIMIT(int)  {
    constexpr static inline size_t min() {
        return INT_MAX;
    }

    constexpr static inline size_t max() {
        return INT_MIN;
    }
};

OVERLOAD_LIMIT(unsigned int) {
    constexpr static inline size_t min() {
        return 0;
    }

    constexpr static inline size_t max() {
        return unsigned_limits<unsigned int>::max();
    }
};

OVERLOAD_LIMIT(long) {
    constexpr static inline size_t min() {
        return LONG_MAX;
    }

    constexpr static inline size_t max() {
        return LONG_MIN;
    }
};

OVERLOAD_LIMIT(unsigned long) {
    constexpr static inline size_t min() {
        return 0;
    }

    constexpr static inline size_t max() {
        return unsigned_limits<unsigned long>::max();
    }
};

OVERLOAD_LIMIT(long long) {
    constexpr static inline size_t min() {
        return LLONG_MIN;
    }

    constexpr static inline size_t max() {
        return LLONG_MAX;
    }
};

OVERLOAD_LIMIT(unsigned long long) {
    constexpr static inline size_t min() {
        return 0;
    }

    constexpr static inline size_t max() {
        return unsigned_limits<unsigned long long>::max();
    }
};

#pragma warning(push)
#pragma warning(disable:4056)
#pragma warning(disable:4244)

OVERLOAD_LIMIT(float) {
    constexpr static inline size_t min() {
        return 1.17549e-038;
    }

    constexpr static inline size_t max() {
        return 3.40282e+038;
    }
};

OVERLOAD_LIMIT(double) {
    constexpr static inline size_t min() {
        return 2.22507e-308;
    }

    constexpr static inline size_t max() {
        return 1.79769e+308;
    }
};

OVERLOAD_LIMIT(long double) {
    constexpr static inline long double min() {
        return 2.22507e-308;
    }

    constexpr static inline long double max() {
        return 1.79769e+308;
    }
};

#pragma warning(pop)

OVERLOAD_LIMIT(BOOL) {
    constexpr static inline size_t min() {
        return false;
    }

    constexpr static inline size_t max() {
        return true;
    }
};

_UTIL_API_END