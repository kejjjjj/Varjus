#pragma once

#include "api/types/types.hpp"
#include "catch_amalgamated.hpp"
#include "runtime/variables.hpp"

#include <string>

IValue* TEST_ExecuteFile(const std::string& srcFile);
