#pragma once

#include "operand.hpp"

#include "linter/modules/references.hpp"

class CRuntimeFunction;
using RuntimeFunction = std::unique_ptr<CRuntimeFunction>;
using ElementIndex = std::size_t;

struct CLambdaOperand final : public IOperand
{
	NONCOPYABLE(CLambdaOperand);

	CLambdaOperand() = default;
	CLambdaOperand(RuntimeFunction&& ptr, VectorOf<CCrossModuleReference>&& captures);
	~CLambdaOperand();

	[[nodiscard]] EOperandBaseType Type() const noexcept override {
		return ot_lambda;
	}

	[[nodiscard]] UniqueAST ToAST() override;

	RuntimeFunction m_pLambda;
	VectorOf<CCrossModuleReference> m_oVariableCaptures;
};
