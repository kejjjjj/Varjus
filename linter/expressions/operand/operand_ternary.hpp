#pragma once

#include "operand.hpp"

struct CTernaryOperand final : public IOperand
{
	NONCOPYABLE(CTernaryOperand);

	CTernaryOperand() = default;
	CTernaryOperand(UniqueAST&& value, UniqueAST&& m_true, UniqueAST&& m_false);
	~CTernaryOperand();

	[[nodiscard]] EOperandBaseType Type() const noexcept override {
		return ot_ternary;
	}

	[[nodiscard]] UniqueAST ToAST() override;

	UniqueAST m_pValue;
	UniqueAST m_pTrue;
	UniqueAST m_pFalse;
};
