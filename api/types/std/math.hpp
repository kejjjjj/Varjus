#pragma once

struct CMathValue final {
	[[nodiscard]] static struct BuiltInMethod_t ConstructMethods();
	[[nodiscard]] static struct BuiltInProperty_t ConstructProperties();
};