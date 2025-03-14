#pragma once

#include "globalEnums.hpp"

#define NONCOPYABLE(className) \
        className(const className&) = delete; \
        className& operator=(const className&) = delete

#if defined(_MSC_VER) 
#define WARNING_PUSH() __pragma(warning(push))
#define WARNING_DISABLE(w) __pragma(warning(disable : w))
#define WARNING_POP() __pragma(warning(pop))
#else
#define WARNING_PUSH()
#define WARNING_POP()
#define WARNING_DISABLE(v)
#endif 

#ifdef _WIN32
#ifdef _WIN64
// 64-bit Windows
using VarjusInt = long long;
using VarjusUInt = unsigned long long;
#else
// 32-bit Windows
using VarjusInt = int;
using VarjusUInt = unsigned int;
#endif
#else
#if __x86_64__ || __ppc64__
// 64-bit non-Windows
using VarjusInt = long long;
using VarjusUInt = unsigned long long;
#else
// 32-bit non-Windows
using VarjusInt = int;
using VarjusUInt = unsigned int;
#endif
#endif

using VarjusDouble = double;

#include "utils/fmt.hpp"