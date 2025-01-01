#pragma once

#include "operand.hpp"

struct CASTOperand final : public IOperand
{
	NONCOPYABLE(CASTOperand);
	CASTOperand(ASTNode&& ptr);
	~CASTOperand();

	[[nodiscard]] EOperandBaseType Type() const noexcept override {
		return ot_abstract_syntax_tree;
	}

	[[nodiscard]] ASTNode ToAST() override;


	ASTNode m_pAST;
};

