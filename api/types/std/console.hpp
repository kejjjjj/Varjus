#pragma once

#include "api/types/object.hpp"
#include "api/types/internal/methods.hpp"

struct CConsoleValue final {
	static BuiltInMethod_t ConstructMethods(); //only called once during init

};
