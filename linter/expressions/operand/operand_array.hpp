#pragma once

#include "operand.hpp"

using ExpressionList = VectorOf<ASTNode>;

struct CArrayOperand final : public IOperand
{
	VARJUS_NONCOPYABLE(CArrayOperand);

	CArrayOperand() = default;
	CArrayOperand(ExpressionList&& ptr);
	~CArrayOperand();

	[[nodiscard]] EOperandBaseType Type() const noexcept override {
		return ot_array;
	}

	[[nodiscard]] ASTNode ToAST() override;


	ExpressionList m_oExpressions;
};
