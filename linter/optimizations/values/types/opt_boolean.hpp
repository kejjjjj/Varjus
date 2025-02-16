#pragma once

#ifdef OPTIMIZATIONS

#include "opt_value.hpp"

class CConstEvalBooleanValue final : public CConstEvalValue<bool>
{
public:
	CConstEvalBooleanValue() = default;

	[[nodiscard]] constexpr EValueType Type() const noexcept override { return t_boolean; }

	void Release() override;
	[[nodiscard]] IConstEvalValue* Copy() override;

	[[nodiscard]] std::shared_ptr<ConstantASTNode> ToAST() const override;

	[[nodiscard]] constexpr bool IsCoerceable() const noexcept override { return true; }

	[[nodiscard]] bool ToBoolean() const override { return Get(); }
	[[nodiscard]] VarjusInt ToInt() const override { return static_cast<VarjusInt>(Get()); }
	[[nodiscard]] VarjusDouble ToDouble() const override { return static_cast<VarjusDouble>(Get()); } 

	[[nodiscard]] CConstEvalBooleanValue* ToCBoolean() override { return this; }

private:
	[[nodiscard]] std::string TypeAsString() const override { return "boolean"s; }
	[[nodiscard]] std::string ValueAsString() const override { return std::to_string(Get()); }
};

#endif