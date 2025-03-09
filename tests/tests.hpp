#pragma once

#include "varjus_api/types/types.hpp"
#include "catch_amalgamated.hpp"
#include "varjus_api/internal/variables.hpp"



IValue* TEST_ExecuteFile(const VarjusString& srcFile);

#define TEST_END(retVal)\
auto allocator = retVal->GetAllocator();\
retVal->Release();\
REQUIRE((allocator && !allocator->HasLeaks()))
