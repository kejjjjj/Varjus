#pragma once
#include "simple.hpp"

class CIntValue : public CValue<std::int64_t>
{
public:
	CIntValue() = default;
	CIntValue(std::int64_t value) : CValue(value) {}
	[[nodiscard]] EValueType Type() const noexcept override { return t_int; };

	[[nodiscard]] IValue* Copy() const override;

	void Release() override;

	[[nodiscard]] bool ToBoolean() const override { return static_cast<bool>(m_oValue); }
	[[nodiscard]] std::int64_t ToInt() const override { return m_oValue; }
	[[nodiscard]] double ToDouble() const override { return static_cast<double>(m_oValue); }

	[[nodiscard]] constexpr bool IsIntegral() const noexcept override { return true; }
	[[nodiscard]] constexpr bool IsCoerceable() const noexcept override { return true; }
	[[nodiscard]] constexpr bool IsArithmetic() const noexcept override { return true; }

private:
	[[nodiscard]] std::string TypeAsString() const override { return "int"s; }
	[[nodiscard]] std::string ValueAsString() const override { return std::to_string(m_oValue); }
};
