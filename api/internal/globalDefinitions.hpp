#pragma once

#include "globalEnums.hpp"

#define NONCOPYABLE(className) \
        className(const className&) = delete; \
        className& operator=(const className&) = delete

#if defined(_MSC_VER) 
#define WARNING_PUSH() #pragma warning(push)
#define WARNING_DISABLE(w)  #pragma warning(disable:w) 
#define WARNING_POP() #pragma warning(pop)
#else
#define WARNING_PUSH()
#define WARNING_POP()
#define WARNING_DISABLE(v)
#endif 

using VarjusInt = long long;
using VarjusUInt = unsigned long long;
using VarjusDouble = double;

