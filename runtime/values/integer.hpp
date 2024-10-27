#pragma once
#include "simple.hpp"

class CIntValue : public IValue
{
public:
	CIntValue() = default;
	CIntValue(std::int64_t value) : m_iValue(value) {}
	[[nodiscard]] EValueType Type() const noexcept override { return t_int; };


	std::int64_t m_iValue;
};
