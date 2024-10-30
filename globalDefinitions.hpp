#pragma once

#include "globalEnums.hpp"

#define NONCOPYABLE(className) \
        className(const className&) = delete; \
        className& operator=(const className&) = delete;
