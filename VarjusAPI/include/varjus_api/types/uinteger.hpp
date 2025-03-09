#pragma once
#include "default.hpp"

class CUIntValue final : public CValue<VarjusUInt>
{
public:
	CUIntValue() = default;

	[[nodiscard]] static CUIntValue* Construct(CProgramRuntime* const runtime, VarjusUInt v);

	[[nodiscard]] EValueType Type() const noexcept override { return t_uint; };

	[[nodiscard]] IValue* Copy() override;

	void Release() override;

	[[nodiscard]] bool ToBoolean() const override { return static_cast<bool>(Get()); }
	[[nodiscard]] VarjusInt ToInt() const override { return static_cast<VarjusInt>(Get()); }
	[[nodiscard]] VarjusUInt ToUInt() const override { return Get(); }
	[[nodiscard]] VarjusDouble ToDouble() const override { return static_cast<VarjusDouble>(Get()); }

	[[nodiscard]] constexpr bool IsIntegral() const noexcept override { return true; }
	[[nodiscard]] constexpr bool IsCoerceable() const noexcept override { return true; }
	[[nodiscard]] constexpr bool IsArithmetic() const noexcept override { return true; }
	[[nodiscard]] constexpr bool IsBooleanConvertible() const noexcept override { return true; }

	[[nodiscard]] CUIntValue* ToCUInt() override { return this; }

private:
	[[nodiscard]] VarjusString TypeAsString() const override { return VSL("uint"); }
	[[nodiscard]] VarjusString ValueAsString() const override { return STD_TO_STRING(Get()); }
};
