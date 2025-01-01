#pragma once

#include "operand.hpp"

struct CTernaryOperand final : public IOperand
{
	NONCOPYABLE(CTernaryOperand);

	CTernaryOperand() = default;
	CTernaryOperand(ASTNode&& value, ASTNode&& m_true, ASTNode&& m_false);
	~CTernaryOperand();

	[[nodiscard]] EOperandBaseType Type() const noexcept override {
		return ot_ternary;
	}

	[[nodiscard]] ASTNode ToAST() override;

	ASTNode m_pValue;
	ASTNode m_pTrue;
	ASTNode m_pFalse;
};
