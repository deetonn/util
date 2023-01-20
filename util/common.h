#pragma once

#ifndef _UTIL_API
#define _UTIL_API namespace util {
#endif

#ifndef _UTIL_API_END
#define _UTIL_API_END }
#endif

#ifndef _W
#define _W(X) L##X
#endif

#ifndef _IMPLICIT
#define _IMPLICIT
#endif

#ifndef _CONSTEXPR
#define _CONSTEXPR constexpr
#endif

#include <exception>
#include <string>
#include <format>

#include <iostream>