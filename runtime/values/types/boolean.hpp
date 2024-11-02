#pragma once
#include "simple.hpp"

class CBooleanValue : public IValue
{
public:
	CBooleanValue() = default;
	CBooleanValue(bool value) : m_bValue(value) {}
	[[nodiscard]] EValueType Type() const noexcept override { return t_boolean; };

	[[nodiscard]] IValue* Copy() const override;

	[[nodiscard]] bool ToBoolean() const override { return m_bValue; }
	[[nodiscard]] std::int64_t ToInt() const override { return static_cast<std::int64_t>(m_bValue); }
	[[nodiscard]] double ToDouble() const override { return static_cast<double>(m_bValue); }

private:
	[[nodiscard]] std::string TypeAsString() const override { return "boolean"s; }
	[[nodiscard]] std::string ValueAsString() const override { return m_bValue ? "true"s : "false"s; }

public:
	bool m_bValue{};
};
