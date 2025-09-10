#pragma once

#include "globalEnums.hpp"

#ifndef VARJUS_NONCOPYABLE
#define VARJUS_NONCOPYABLE(className) \
        className(const className&) = delete; \
        className& operator=(const className&) = delete
#endif

#if defined(_MSC_VER) 
#define VARJUS_WARNING_PUSH() __pragma(warning(push))
#define VARJUS_WARNING_DISABLE(w) __pragma(warning(disable : w))
#define VARJUS_WARNING_POP() __pragma(warning(pop))
#else
#define VARJUS_WARNING_PUSH()
#define VARJUS_WARNING_POP()
#define VARJUS_WARNING_DISABLE(v)
#endif 

#if defined(_WIN32)
        #if defined(_WIN64)
                // 64-bit Windows
                using VarjusInt = long long;
                using VarjusUInt = unsigned long long;
        #else
                // 32-bit Windows
                using VarjusInt = int;
                using VarjusUInt = unsigned int;
        #endif
#elif defined(__APPLE__) && defined(__MACH__)
        #include <TargetConditionals.h>
        #if defined(__x86_64__) || defined(__aarch64__)
                // 64-bit macOS
                using VarjusInt = long long;
                using VarjusUInt = unsigned long long;
        #else
                // 32-bit macOS
                using VarjusInt = int;
                using VarjusUInt = unsigned int;
        #endif
#else
        #if defined(__x86_64__) || defined(__ppc64__)
                // 64-bit non-Windows, non-macOS
                using VarjusInt = long long;
                using VarjusUInt = unsigned long long;
        #else
                // 32-bit non-Windows, non-macOS
                using VarjusInt = int;
                using VarjusUInt = unsigned int;
        #endif
#endif

using VarjusDouble = double;

#include "utils/fmt.hpp"