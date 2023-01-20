#pragma once

#include "common.h"

_UTIL_API

template<class _Ty, class _Ret>
concept Add = requires(_Ty t) {
    { t.add((_Ret)0) } -> std::same_as<_Ret>;
};

template<class _Ty, class _Ret>
concept Sub = requires (_Ty t) {
    { t.subtract((_Ret)0) } -> std::same_as<_Ret>;
};

template<class _Ty, class _Ret>
concept Div = requires (_Ty t) {
    { t.divide((_Ret)1) } -> std::same_as<_Ret>;
};

template<class _Ty, class _Ret>
concept Mul = requires (_Ty t) {
    { t.multiply((_Ret)0) } -> std::same_as<_Ret>;
};

template<class _Ty, class _Ret>
concept BasicMathCapable =
    Add<_Ty, _Ret> && Sub<_Ty, _Ret> &&
    Div<_Ty, _Ret> && Mul<_Ty, _Ret>;

_UTIL_API_END