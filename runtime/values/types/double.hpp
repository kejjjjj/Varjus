#pragma once
#include "simple.hpp"


class CDoubleValue : public CValue<double>
{
public:
	CDoubleValue() = default;
	CDoubleValue(double value) : CValue(value) {}
	[[nodiscard]] EValueType Type() const noexcept override { return t_double; };
	
	[[nodiscard]] IValue* Copy() override;

	void Release() override;

	[[nodiscard]] bool ToBoolean() const override { return static_cast<bool>(m_oValue); }
	[[nodiscard]] std::int64_t ToInt() const override { return static_cast<std::int64_t>(m_oValue); }
	[[nodiscard]] double ToDouble() const override { return m_oValue; }

	[[nodiscard]] constexpr bool IsCoerceable() const noexcept override { return true; }
	[[nodiscard]] constexpr bool IsArithmetic() const noexcept override { return true; }
	[[nodiscard]] constexpr bool IsBooleanConvertible() const noexcept override { return true; }

private:
	[[nodiscard]] std::string TypeAsString() const override { return "double"s; }
	[[nodiscard]] std::string ValueAsString() const override { return std::to_string(m_oValue); }
};
