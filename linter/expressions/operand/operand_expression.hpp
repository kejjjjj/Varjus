#pragma once

#include "operand.hpp"

struct CASTOperand final : public IOperand
{
	NONCOPYABLE(CASTOperand);
	CASTOperand(UniqueAST&& ptr);
	~CASTOperand();

	[[nodiscard]] EOperandBaseType Type() const noexcept override {
		return ot_abstract_syntax_tree;
	}

	[[nodiscard]] UniqueAST ToAST() override;


	UniqueAST m_pAST;
};

