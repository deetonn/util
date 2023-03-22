#pragma once

#include "common.h"

#include <ranges>
#include <random>

_UTIL_API

template<class T>
class implements : public T {};

template<typename ...Classes>
class implements_many : public Classes...{};

static constexpr inline auto True  = 1;
static constexpr inline auto False = 0;

constexpr auto truthy(auto value) -> BOOL {
    if (value)
        return True;
    return False;
}
constexpr auto falsey(auto value) -> BOOL {
    return !truthy(value);
}

using RandT =
#if defined (X64)
std::mt19937_64;
#else
std::mt19937;
#endif

static inline RandT* u_Rand = nullptr;

inline auto RandMt() -> RandT& {
    _UTL panic_if(!u_Rand, "global random device has not been initialized yet");
    return *u_Rand;
}

/*
* Returns a random number between _Lower & _Upper.
* @param _Lower: The lower bound number, must be smaller than _Upper.
* @param _Upper: The upper bound number, must be larger than _Lower
* @return: The random number as typename util::RandT::result_type.
*/
[[nodiscard("could cause a pointless allocation")]]
inline typename RandT::result_type rand(
    RandT::result_type _Lower = 0,
    RandT::result_type _Upper = -1
)
{
#if defined (_DEBUG)
    _STL_VERIFY(_Upper > _Lower, "rand range out of bounds");
#endif

    static auto _Device = std::random_device{};
    if (!u_Rand) {
        // dont bother freeing this memory, it should always be alive.
        // not on the stack because sizeof RandT == 5000 ... 
        u_Rand = new RandT{_Device()};
    }

    auto& _Myref = RandMt();
    auto dist = std::uniform_int_distribution<
        typename RandT::result_type>(_Lower, _Upper);

    return dist(_Myref);
}

[[nodiscard("could cause a pointless allocation")]]
inline BOOL randb() {
    return static_cast<BOOL>(rand(0, 1));
}

std::chrono::microseconds measure(std::function<void()> fn) {
    auto now = std::chrono::high_resolution_clock::now();
    fn();
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(end - now);
}

_UTIL_API_END