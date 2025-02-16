#pragma once
#include "default.hpp"


class CDoubleValue final : public CValue<VarjusDouble>
{
public:
	CDoubleValue() = default;
	
	[[nodiscard]] static CDoubleValue* Construct(VarjusDouble v);

	[[nodiscard]] EValueType Type() const noexcept override { return t_double; };
	
	[[nodiscard]] IValue* Copy() override;

	virtual void Release() override;

	[[nodiscard]] bool ToBoolean() const override { return static_cast<bool>(Get()); }
	[[nodiscard]] VarjusInt ToInt() const override { return static_cast<VarjusInt>(Get()); }
	[[nodiscard]] VarjusUInt ToUInt() const override { return static_cast<VarjusUInt>(Get()); }
	[[nodiscard]] VarjusDouble ToDouble() const override { return Get(); }

	[[nodiscard]] constexpr bool IsCoerceable() const noexcept override { return true; }
	[[nodiscard]] constexpr bool IsArithmetic() const noexcept override { return true; }
	[[nodiscard]] constexpr bool IsBooleanConvertible() const noexcept override { return true; }

	[[nodiscard]] CDoubleValue* ToCDouble() override { return this; }

private:
	[[nodiscard]] std::string TypeAsString() const override { return "double"s; }
	[[nodiscard]] std::string ValueAsString() const override { return std::to_string(Get()); }
};
