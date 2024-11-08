#pragma once

#include "globalEnums.hpp"

#define NONCOPYABLE(className) \
        className(const className&) = delete; \
        className& operator=(const className&) = delete;


#define IS_IMMEDIATE(v) v >= TokenType::tt_undefined && v <= TokenType::tt_string