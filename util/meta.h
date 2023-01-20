#pragma once

#include "common.h"

#include <ranges>

_UTIL_API

template<class T>
class implements : public T {};

template<typename ...Classes>
class implements_many : public Classes...{};

static constexpr inline auto True  = 1;
static constexpr inline auto False = 0;

constexpr auto truthy(auto value) -> bool {
    if (value)
        return True;
    return False;
}

constexpr auto falsey(auto value) -> bool {
    return !truthy(value);
}

std::ranges::as_rvalue_view

_UTIL_API_END