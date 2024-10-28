#pragma once
#include "simple.hpp"


class CDoubleValue : public IValue
{
public:
	CDoubleValue() = default;
	CDoubleValue(double value) : m_dValue(value) {}
	[[nodiscard]] EValueType Type() const noexcept override { return t_double; };
	
private:
	std::string TypeAsString() const override { return "int"s; }
	std::string ValueAsString() const override { return std::to_string(m_dValue); }

public:
	double m_dValue;
};
