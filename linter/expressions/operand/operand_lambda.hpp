#pragma once

#include "operand.hpp"

class CRuntimeFunction;
using RuntimeFunction = std::unique_ptr<CRuntimeFunction>;
using ElementIndex = std::size_t;

struct CLambdaOperand final : public IOperand
{
	NONCOPYABLE(CLambdaOperand);

	CLambdaOperand() = default;
	CLambdaOperand(RuntimeFunction&& ptr, VectorOf<ElementIndex>&& captures);
	~CLambdaOperand();

	[[nodiscard]] EOperandBaseType Type() const noexcept override {
		return ot_lambda;
	}

	[[nodiscard]] UniqueAST ToAST() override;

	RuntimeFunction m_pLambda;
	VectorOf<ElementIndex> m_oVariableCaptures;

};
