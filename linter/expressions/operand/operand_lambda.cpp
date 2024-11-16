#include "operand.hpp"
#include "operand_lambda.hpp"

#include "linter/expressions/ast.hpp"
#include "linter/token.hpp"
#include "linter/functions/function.hpp"
#include "linter/scopes/scope.hpp"

#include "runtime/structure.hpp"

#include <cassert>

std::unique_ptr<IOperand> CLinterOperand::ParseLambda()
{
	assert(!IsEndOfBuffer() && (*m_iterPos)->Type() == tt_fn);

	std::advance(m_iterPos, 1); // skip fn

	CFunctionLinter fnLinter(m_iterPos, m_iterEnd, m_pScope, m_pOwner);
	fnLinter.m_pThisStack->m_pLowerFunction = dynamic_cast<CStack*>(m_pOwner)->GetGlobalFunction();

	if (!fnLinter.ParseFunctionParameters())
		return nullptr;
	


	if (!fnLinter.ParseFunctionScope()) {
		return nullptr;
	}

	std::advance(m_iterPos, 1);

	fnLinter.m_oFunctionName = "lambda";
	fnLinter.m_pThisStack->m_pFunction = fnLinter.ToFunction();

	return std::make_unique<CLambdaOperand>(fnLinter.ToRuntimeFunction());
}

CLambdaOperand::CLambdaOperand(RuntimeFunction&& ptr) : m_pLambda(std::move(ptr)){}
CLambdaOperand::~CLambdaOperand() = default;

UniqueAST CLambdaOperand::ToAST() {
	return std::make_unique<LambdaASTNode>(std::move(m_pLambda));
}