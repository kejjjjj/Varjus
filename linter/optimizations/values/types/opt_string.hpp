#pragma once

#ifdef OPTIMIZATIONS

#include "opt_value.hpp"

class CConstEvalStringValue final : public CConstEvalValue<std::string>
{
public:
	CConstEvalStringValue() = default;

	[[nodiscard]] constexpr EValueType Type() const noexcept override { return t_string; }

	void Release() override;
	[[nodiscard]] IConstEvalValue* Copy() override;

	[[nodiscard]] std::shared_ptr<ConstantASTNode> ToAST() const override;

	[[nodiscard]] constexpr bool IsCoerceable() const noexcept override { return false; }

	[[nodiscard]] bool ToBoolean() const override { return !m_oValue.empty(); }
	[[nodiscard]] std::int64_t ToInt() const override { return static_cast<std::int64_t>(ToBoolean()); }
	[[nodiscard]] double ToDouble() const override { return static_cast<double>(ToBoolean()); }
	[[nodiscard]] const std::string& ToString() const override { return m_oValue; }

	[[nodiscard]] CConstEvalStringValue* ToCString() override { return this; }

private:
	[[nodiscard]] std::string TypeAsString() const override { return "string"s; }
	[[nodiscard]] std::string ValueAsString() const override { return m_oValue; }
};

#endif