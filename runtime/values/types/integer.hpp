#pragma once
#include "simple.hpp"

class CIntValue : public CValue<std::int64_t>
{
public:
	CIntValue() = default;
	CIntValue(std::int64_t value) : CValue(value) {}
	[[nodiscard]] EValueType Type() const noexcept override { return t_int; };

	[[nodiscard]] IValue* Copy() override;

	virtual void Release() override;

	[[nodiscard]] bool ToBoolean() const override { return static_cast<bool>(Get()); }
	[[nodiscard]] std::int64_t ToInt() const override { return Get(); }
	[[nodiscard]] double ToDouble() const override { return static_cast<double>(Get()); }

	[[nodiscard]] constexpr bool IsIntegral() const noexcept override { return true; }
	[[nodiscard]] constexpr bool IsCoerceable() const noexcept override { return true; }
	[[nodiscard]] constexpr bool IsArithmetic() const noexcept override { return true; }
	[[nodiscard]] constexpr bool IsBooleanConvertible() const noexcept override { return true; }

private:
	[[nodiscard]] std::string TypeAsString() const override { return "int"s; }
	[[nodiscard]] std::string ValueAsString() const override { return std::to_string(Get()); }
};
