#pragma once

#include "catch_amalgamated.hpp"
#include "runtime/values/types/types.hpp"
#include "runtime/variables.hpp"
#include <string>

IValue* TEST_ExecuteFile(const std::string& srcFile);
