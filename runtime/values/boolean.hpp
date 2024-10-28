#pragma once
#include "simple.hpp"

class CBooleanValue : public IValue
{
public:
	CBooleanValue() = default;
	CBooleanValue(bool value) : m_bValue(value) {}
	[[nodiscard]] EValueType Type() const noexcept override { return t_boolean; };

private:
	std::string TypeAsString() const override { return "boolean"s; }
	std::string ValueAsString() const override { return m_bValue ? "true"s : "false"s; }

public:
	bool m_bValue;
};
