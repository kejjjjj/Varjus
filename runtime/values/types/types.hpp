#pragma once

#include "simple.hpp"
#include "boolean.hpp"
#include "integer.hpp"
#include "double.hpp"
#include "string.hpp"

#define IVALUE_UNION \
union { \
IValue* undefinedValue; \
CBooleanValue* booleanValue; \
CIntValue* intValue; \
CDoubleValue* doubleValue; \
CStringValue* stringValue; \
}
