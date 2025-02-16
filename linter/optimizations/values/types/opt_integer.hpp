#pragma once

#ifdef OPTIMIZATIONS


#include "opt_value.hpp"

class CConstEvalIntValue final : public CConstEvalValue<VarjusInt>
{
public:
	CConstEvalIntValue() = default;

	[[nodiscard]] constexpr EValueType Type() const noexcept override { return t_int; }

	void Release() override;
	[[nodiscard]] IConstEvalValue* Copy() override;

	[[nodiscard]] std::shared_ptr<ConstantASTNode> ToAST() const override;

	[[nodiscard]] constexpr bool IsCoerceable() const noexcept override { return true; }

	[[nodiscard]] bool ToBoolean() const override { return static_cast<bool>(Get()); }
	[[nodiscard]] VarjusInt ToInt() const override { return Get(); }
	[[nodiscard]] VarjusDouble ToDouble() const override { return static_cast<VarjusDouble>(Get()); }

	[[nodiscard]] CConstEvalIntValue* ToCInt() override { return this; }

private:
	[[nodiscard]] std::string TypeAsString() const override { return "int"s; }
	[[nodiscard]] std::string ValueAsString() const override { return std::to_string(Get()); }
};

#endif