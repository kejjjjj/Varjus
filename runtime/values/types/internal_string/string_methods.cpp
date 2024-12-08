
#include "runtime/values/types/types.hpp"
#include "runtime/runtime.hpp"
#include "runtime/variables.hpp"
#include "runtime/exceptions/exception.hpp"
#include "runtime/values/simple_operators.hpp"

#include <algorithm>

IValue* CStringValue::ToUpper([[maybe_unused]] CFunction* const thisFunction, [[maybe_unused]] const IValues& newValue)
{
	std::string v = Internal()->GetString();
	std::ranges::transform(v, v.begin(), [](std::int8_t c) { 
		return static_cast<std::int8_t>(std::toupper(static_cast<std::int32_t>(c))); });
	return Construct(v);
}
IValue* CStringValue::ToLower([[maybe_unused]] CFunction* const thisFunction, [[maybe_unused]] const IValues& newValue)
{
	std::string v = Internal()->GetString();
	std::ranges::transform(v, v.begin(), [](std::int8_t c) {
		return static_cast<std::int8_t>(std::tolower(static_cast<std::int32_t>(c))); });
	return Construct(v);
}