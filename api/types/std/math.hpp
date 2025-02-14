#pragma once

#include "api/types/object.hpp"
#include "api/types/internal/methods.hpp"

struct CMathValue final {
	static BuiltInMethod_t ConstructMethods(); //only called once during init
};