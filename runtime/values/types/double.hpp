#pragma once
#include "simple.hpp"


class CDoubleValue : public IValue
{
public:
	CDoubleValue() = default;
	CDoubleValue(double value) : m_dValue(value) {}
	[[nodiscard]] EValueType Type() const noexcept override { return t_double; };
	
	[[nodiscard]] IValue* Copy() const override;

	[[nodiscard]] bool ToBoolean() const override { return static_cast<bool>(m_dValue); }
	[[nodiscard]] std::int64_t ToInt() const override { return static_cast<std::int64_t>(m_dValue); }
	[[nodiscard]] double ToDouble() const override { return m_dValue; }

private:
	[[nodiscard]] std::string TypeAsString() const override { return "int"s; }
	[[nodiscard]] std::string ValueAsString() const override { return std::to_string(m_dValue); }

public:
	double m_dValue;
};
