#pragma once

#include "operand.hpp"

using ExpressionList = VectorOf<UniqueAST>;

struct CArrayOperand final : public IOperand
{
	NONCOPYABLE(CArrayOperand);

	CArrayOperand() = default;
	CArrayOperand(ExpressionList&& ptr);
	~CArrayOperand();

	[[nodiscard]] EOperandBaseType Type() const noexcept override {
		return ot_array;
	}

	[[nodiscard]] UniqueAST ToAST() override;


	ExpressionList m_oExpressions;
};
