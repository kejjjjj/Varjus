#include "operand.hpp"
#include "operand_lambda.hpp"

#include "linter/expressions/ast.hpp"
#include "linter/token.hpp"
#include "linter/functions/function.hpp"
#include "linter/scopes/scope.hpp"
#include "linter/error.hpp"

#include "runtime/structure.hpp"

#include <cassert>

std::unique_ptr<IOperand> CLinterOperand::ParseLambda()
{
	assert(!IsEndOfBuffer() && (*m_iterPos)->Type() == tt_fn);

	auto& oldIter = m_iterPos;

	if (m_pOwner->IsLocalFunction()) {
		CLinterErrors::PushError("nested lambdas are not supported", GetIteratorSafe()->m_oSourcePosition);
		return nullptr;
	}

	std::advance(m_iterPos, 1); // skip fn

	CFunctionLinter fnLinter(m_iterPos, m_iterEnd, m_pScope, m_pOwner);
	fnLinter.m_pThisStack->m_pLowerFunction = m_pOwner->IsGlobalMemory() 
		? nullptr 
		: m_pOwner->ToStack()->GetGlobalFunction();

	fnLinter.m_pThisScope->MakeLoopScope(false);

	if (!fnLinter.ParseFunctionParameters())
		return nullptr;
	
	if (!fnLinter.ParseFunctionScope()) {
		return nullptr;
	}

	std::advance(m_iterPos, 1);

	if (m_pOwner->IsHoisting())
		return nullptr;

	fnLinter.m_oFunctionName = "lambda";
	fnLinter.m_pThisStack->m_pFunction = fnLinter.ToFunction();

	VectorOf<CCrossModuleReference> refs = m_pOwner->IsStack()
		? fnLinter.GetSharedOwnershipVariables(m_pOwner->ToStack())
		: VectorOf<CCrossModuleReference>{};

	auto&& ptr = std::make_unique<CLambdaOperand>(fnLinter.ToRuntimeFunction(), std::move(refs));
	ptr->m_oCodePosition = (*oldIter)->m_oSourcePosition;
	return ptr;
}

CLambdaOperand::CLambdaOperand(RuntimeFunction&& ptr, VectorOf<CCrossModuleReference>&& captures)
	: m_pLambda(std::move(ptr)), m_oVariableCaptures(std::move(captures)) {}
CLambdaOperand::~CLambdaOperand() = default;

UniqueAST CLambdaOperand::ToAST() {
	return std::make_unique<LambdaASTNode>(m_oCodePosition, std::move(m_pLambda), std::move(m_oVariableCaptures));
}