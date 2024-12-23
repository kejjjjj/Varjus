#pragma once
#include "simple.hpp"


class CDoubleValue final : public CValue<double>
{
public:
	CDoubleValue() = default;
	[[nodiscard]] EValueType Type() const noexcept override { return t_double; };
	
	[[nodiscard]] IValue* Copy() override;

	virtual void Release() override;

	[[nodiscard]] bool ToBoolean() const override { return static_cast<bool>(Get()); }
	[[nodiscard]] std::int64_t ToInt() const override { return static_cast<std::int64_t>(Get()); }
	[[nodiscard]] double ToDouble() const override { return Get(); }

	[[nodiscard]] constexpr bool IsCoerceable() const noexcept override { return true; }
	[[nodiscard]] constexpr bool IsArithmetic() const noexcept override { return true; }
	[[nodiscard]] constexpr bool IsBooleanConvertible() const noexcept override { return true; }

	[[nodiscard]] CDoubleValue* ToCDouble() override { return this; }


private:
	[[nodiscard]] std::string TypeAsString() const override { return "double"s; }
	[[nodiscard]] std::string ValueAsString() const override { return std::to_string(Get()); }
};
