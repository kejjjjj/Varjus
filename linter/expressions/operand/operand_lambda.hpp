#pragma once

#include "operand.hpp"

class CRuntimeFunction;
using RuntimeFunction = std::unique_ptr<CRuntimeFunction>;

struct CLambdaOperand final : public IOperand
{
	NONCOPYABLE(CLambdaOperand);

	CLambdaOperand() = default;
	CLambdaOperand(RuntimeFunction&& ptr);
	~CLambdaOperand();

	[[nodiscard]] EOperandBaseType Type() const noexcept override {
		return ot_lambda;
	}

	[[nodiscard]] UniqueAST ToAST() override;

	RuntimeFunction m_pLambda;
};
