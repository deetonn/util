#pragma once

#include "common.h"

_UTIL_API

template<class _Ty, class _Ret>
concept Add = requires(_Ty t) {
    { t.operator+((_Ret)0) } -> std::same_as<_Ret>;
};

template<class _Ty, class _Ret>
concept Sub = requires (_Ty t) {
    { t.operator-((_Ret)0) } -> std::same_as<_Ret>;
};

template<class _Ty, class _Ret>
concept Div = requires (_Ty t) {
    { t.operator/((_Ret)1) } -> std::same_as<_Ret>;
};

template<class _Ty, class _Ret>
concept Mul = requires (_Ty t) {
    { t.operator*((_Ret)0) } -> std::same_as<_Ret>;
};

template<class _Ty, class _Ret>
concept BasicMathCapable =
    Add<_Ty, _Ret> && Sub<_Ty, _Ret> &&
    Div<_Ty, _Ret> && Mul<_Ty, _Ret>;

template<class _Ty>
concept iterable = requires (_Ty t) {
    { t.begin() } -> std::same_as<typename _Ty::template const_iterator<typename _Ty::value_type>>;
    { t.end() } -> std::same_as<typename _Ty::template const_iterator<typename _Ty::value_type>>;
    { t.operator*() } -> std::same_as<const typename _Ty::value_type>;
    { t.operator++() } -> std::same_as<_Ty>;
};

template<class _Ty, class _El>
concept allocator = requires (_Ty t) {
    { t.template alloc<_El>() } -> std::same_as<_El*>;
    { t.template destroy<_El>((_El*)0) } -> std::same_as<void>;
    { t.total() } -> std::same_as<size_t>;
    { t.template alloc_many<_El>((size_t)8) } -> std::same_as<_El*>;
} && std::is_default_constructible_v<_Ty>;

_UTIL_API_END