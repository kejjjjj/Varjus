#pragma once

struct CMathValue final {
	[[nodiscard]] static struct BuiltInMethod_t ConstructMethods(class CProgramInformation* const info);
	[[nodiscard]] static struct BuiltInProperty_t ConstructProperties(class CProgramInformation* const info);
};