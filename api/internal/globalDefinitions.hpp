#pragma once

#include "globalEnums.hpp"

#define NONCOPYABLE(className) \
        className(const className&) = delete; \
        className& operator=(const className&) = delete;



using VarjusInt = long long;
using VarjusUInt = unsigned long long;
using VarjusDouble = double;

