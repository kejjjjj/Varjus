#pragma once

#ifdef OPTIMIZATIONS

#include "opt_value.hpp"

class CConstEvalDoubleValue final : public CConstEvalValue<VarjusDouble>
{
public:
	CConstEvalDoubleValue() = default;

	[[nodiscard]] constexpr EValueType Type() const noexcept override { return t_double; }

	void Release() override;
	[[nodiscard]] IConstEvalValue* Copy() override;

	[[nodiscard]] std::shared_ptr<ConstantASTNode> ToAST() const override;

	[[nodiscard]] constexpr bool IsCoerceable() const noexcept override { return true; }

	[[nodiscard]] bool ToBoolean() const override { return static_cast<bool>(Get()); }
	[[nodiscard]] VarjusInt ToInt() const override { return static_cast<VarjusInt>(Get()); }
	[[nodiscard]] VarjusDouble ToDouble() const override { return Get(); }

	[[nodiscard]] CConstEvalDoubleValue* ToCDouble() override { return this; }

private:
	[[nodiscard]] std::string TypeAsString() const override { return "VarjusDouble"s; }
	[[nodiscard]] std::string ValueAsString() const override { return std::to_string(Get()); }
};

#endif