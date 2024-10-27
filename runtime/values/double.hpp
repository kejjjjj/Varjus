#pragma once
#include "simple.hpp"


class CDoubleValue : public IValue
{
public:
	CDoubleValue() = default;
	CDoubleValue(double value) : m_dValue(value) {}
	[[nodiscard]] EValueType Type() const noexcept override { return t_double; };
	
	double m_dValue;
};
