#pragma once

#include "runtime/values/types/object.hpp"
#include "runtime/values/types/internal/methods.hpp"

struct CMathValue final {
	static BuiltInMethod_t ConstructMethods(); //only called once during init
};