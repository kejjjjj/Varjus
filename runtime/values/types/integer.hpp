#pragma once
#include "simple.hpp"

class CIntValue : public IValue
{
public:
	CIntValue() = default;
	CIntValue(std::int64_t value) : m_iValue(value) {}
	[[nodiscard]] EValueType Type() const noexcept override { return t_int; };

	[[nodiscard]] IValue* Copy() const override;

	[[nodiscard]] bool ToBoolean() const override { return static_cast<bool>(m_iValue); }
	[[nodiscard]] std::int64_t ToInt() const override { return m_iValue; }
	[[nodiscard]] double ToDouble() const override { return static_cast<double>(m_iValue); }

private:
	[[nodiscard]] std::string TypeAsString() const override { return "int"s; }
	[[nodiscard]] std::string ValueAsString() const override { return std::to_string(m_iValue); }

public:
	std::int64_t m_iValue{};
};
