#pragma once

#ifdef OPTIMIZATIONS

#include "opt_value.hpp"

class CConstEvalStringValue final : public CConstEvalValue<VarjusString>
{
public:
	CConstEvalStringValue() = default;

	[[nodiscard]] constexpr EValueType Type() const noexcept override { return t_string; }

	void Release() override;
	[[nodiscard]] IConstEvalValue* Copy() override;

	[[nodiscard]] std::shared_ptr<ConstantASTNode> ToAST() const override;

	[[nodiscard]] constexpr bool IsCoerceable() const noexcept override { return false; }

	[[nodiscard]] bool ToBoolean() const override { return !m_oValue.empty(); }
	[[nodiscard]] VarjusInt ToInt() const override { return static_cast<VarjusInt>(ToBoolean()); }
	[[nodiscard]] VarjusDouble ToDouble() const override { return static_cast<VarjusDouble>(ToBoolean()); }
	[[nodiscard]] const VarjusString& ToString() const override { return m_oValue; }

	[[nodiscard]] CConstEvalStringValue* ToCString() override { return this; }

private:
	[[nodiscard]] VarjusString TypeAsString() const override { return VSL("string"); }
	[[nodiscard]] VarjusString ValueAsString() const override { return m_oValue; }
};

#endif