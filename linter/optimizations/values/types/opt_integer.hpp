#pragma once

#include "opt_value.hpp"

class CConstEvalIntValue final : public CConstEvalValue<std::int64_t>
{
public:
	CConstEvalIntValue() = default;

	[[nodiscard]] constexpr EValueType Type() const noexcept override { return t_int; }
	[[nodiscard]] std::int64_t ToInt() const override { return m_oValue; }

	void Release() override;
	[[nodiscard]] IConstEvalValue* Copy() override;

	[[nodiscard]] constexpr bool IsCoerceable() const noexcept override { return true; }

private:
	[[nodiscard]] std::string TypeAsString() const override { return "int"s; }
	[[nodiscard]] std::string ValueAsString() const override { return std::to_string(Get()); }
};

