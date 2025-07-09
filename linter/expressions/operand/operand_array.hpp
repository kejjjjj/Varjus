#pragma once

#include "operand.hpp"

using __ExpressionList = VectorOf<ASTNode>;

struct CArrayOperand final : public IOperand
{
	VARJUS_NONCOPYABLE(CArrayOperand);

	CArrayOperand() = default;
	CArrayOperand(__ExpressionList&& ptr);
	~CArrayOperand();

	[[nodiscard]] EOperandBaseType Type() const noexcept override {
		return ot_array;
	}

	[[nodiscard]] ASTNode ToAST() override;


	__ExpressionList m_oExpressions;
};
