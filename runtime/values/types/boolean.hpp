#pragma once
#include "simple.hpp"

class CBooleanValue : public CValue<bool>
{
public:
	CBooleanValue() = default;
	CBooleanValue(bool value) : CValue(value) {}
	[[nodiscard]] EValueType Type() const noexcept override { return t_boolean; };

	[[nodiscard]] IValue* Copy() override;

	void Release() override;

	[[nodiscard]] bool ToBoolean() const override { return m_oValue; }
	[[nodiscard]] std::int64_t ToInt() const override { return static_cast<std::int64_t>(m_oValue); }
	[[nodiscard]] double ToDouble() const override { return static_cast<double>(m_oValue); }

	[[nodiscard]] constexpr bool IsIntegral() const noexcept override { return true; }
	[[nodiscard]] constexpr bool IsCoerceable() const noexcept override { return true; }
	[[nodiscard]] constexpr bool IsArithmetic() const noexcept override { return true; }

private:
	[[nodiscard]] std::string TypeAsString() const override { return "boolean"s; }
	[[nodiscard]] std::string ValueAsString() const override { return m_oValue ? "true"s : "false"s; }

public:
};
