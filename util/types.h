#pragma once

#include "common.h"

#define _Conditional_Noexcept(_Condition_) noexcept (_Condition_)
#define _Maybe_Constantdef template<BOOL _Const_Init = false>
#define UTIL_DEF

#define _Nodiscard nodiscard
#ifndef _NODISCARD
#define _NODISCARD [[_Nodiscard]]
#endif
#define _UTIL_NODISCARD(x) [[_Nodiscard(x)]]
#define UTIL_NODISCARD _UTIL_NODISCARD("ignoring this return value is not recommended.")

#define _Likely_
#define _Unlikely_
#define _FORBIDDEN [[deprecated("This api is forbidden")]]

_UTIL_API

using sstring  = std::string;
using swstring = std::wstring;

using u8 = unsigned char;
using i8 = char;

using u16 = unsigned short;
using i16 = short;

using u32 = unsigned int;
using i32 = int;

using u64 = unsigned long long;
using i64 = long long;

using BOOL = bool;

template<class T>
using NativeArray = T*;

template<typename T>
using remove_reference = std::remove_reference<T>;

_UTIL_API_END