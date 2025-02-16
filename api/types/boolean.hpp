#pragma once
#include "default.hpp"

class CBooleanValue final : public CValue<bool>
{
public:

	CBooleanValue() = default;
	[[nodiscard]] EValueType Type() const noexcept override { return t_boolean; };

	[[nodiscard]] static CBooleanValue* Construct(bool v);

	[[nodiscard]] IValue* Copy() override;

	virtual void Release() override;

	[[nodiscard]] bool ToBoolean() const override { return Get(); }
	[[nodiscard]] VarjusInt ToInt() const override { return static_cast<VarjusInt>(Get()); }
	[[nodiscard]] VarjusUInt ToUInt() const override { return static_cast<VarjusUInt>(Get()); }
	[[nodiscard]] VarjusDouble ToDouble() const override { return static_cast<VarjusDouble>(Get()); }

	[[nodiscard]] constexpr bool IsIntegral() const noexcept override { return true; }
	[[nodiscard]] constexpr bool IsCoerceable() const noexcept override { return true; }
	[[nodiscard]] constexpr bool IsArithmetic() const noexcept override { return true; }
	[[nodiscard]] constexpr bool IsBooleanConvertible() const noexcept override { return true; }

	[[nodiscard]] CBooleanValue* ToCBoolean() override { return this; }


private:
	[[nodiscard]] std::string TypeAsString() const override { return "boolean"s; }
	[[nodiscard]] std::string ValueAsString() const override { return Get() ? "true"s : "false"s; }

public:
};
