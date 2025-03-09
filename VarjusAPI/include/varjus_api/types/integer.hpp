#pragma once
#include "default.hpp"

class CIntValue final : public CValue<VarjusInt>
{
public:
	CIntValue() = default;

	[[nodiscard]] static CIntValue* Construct(CProgramRuntime* const runtime, VarjusInt v);

	[[nodiscard]] EValueType Type() const noexcept override { return t_int; };

	[[nodiscard]] IValue* Copy() override;

	void Release() override;

	[[nodiscard]] bool ToBoolean() const override { return static_cast<bool>(Get()); }
	[[nodiscard]] VarjusInt ToInt() const override { return Get(); }
	[[nodiscard]] VarjusUInt ToUInt() const override { return static_cast<VarjusUInt>(Get()); }
	[[nodiscard]] VarjusDouble ToDouble() const override { return static_cast<VarjusDouble>(Get()); }

	[[nodiscard]] constexpr bool IsIntegral() const noexcept override { return true; }
	[[nodiscard]] constexpr bool IsCoerceable() const noexcept override { return true; }
	[[nodiscard]] constexpr bool IsArithmetic() const noexcept override { return true; }
	[[nodiscard]] constexpr bool IsBooleanConvertible() const noexcept override { return true; }

	[[nodiscard]] CIntValue* ToCInt() override { return this; }

private:
	[[nodiscard]] VarjusString TypeAsString() const override { return VSL("int"); }
	[[nodiscard]] VarjusString ValueAsString() const override { return STD_TO_STRING(Get()); }
};
