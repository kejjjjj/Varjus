#pragma once

#include "api/types/types.hpp"
#include "catch_amalgamated.hpp"
#include "api/internal/variables.hpp"

#include <string>

IValue* TEST_ExecuteFile(const std::string& srcFile);

#define TEST_END(retVal)\
auto allocator = retVal->GetAllocator();\
retVal->Release();\
REQUIRE((allocator && !allocator->HasLeaks()))
