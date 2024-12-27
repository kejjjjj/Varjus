#include "operand.hpp"
#include "operand_lambda.hpp"

#include "linter/expressions/ast.hpp"
#include "linter/token.hpp"
#include "linter/functions/function.hpp"
#include "linter/scopes/scope.hpp"
#include "linter/error.hpp"
#include "linter/expressions/expression.hpp"
#include "linter/statements/return/return.hpp"

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

/***********************************************************************
 > 
***********************************************************************/
CLambdaChecker::CLambdaChecker(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const stack)
	: CLinterSingle(pos, end), m_pScope(scope), m_pOwner(stack) {

	assert(m_iterPos != m_iterEnd);

}
CLambdaChecker::~CLambdaChecker() = default;

Success CLambdaChecker::Parse(std::optional<PairMatcher>& m_oEndOfExpression, CExpressionList* expression, EvaluationType evalType)
{
	LinterIterator initialPos = m_iterPos;

	if (!ParseInternal(m_oEndOfExpression, expression, evalType)) {
		m_iterPos = initialPos;
		return failure;
	}


	return success;
}
Success CLambdaChecker::ParseInternal(
	std::optional<PairMatcher>& m_oEndOfExpression, [[maybe_unused]] CExpressionList* expression, [[maybe_unused]] EvaluationType evalType)
{
	//short lambdas must start with () parms
	if (EndOfExpression(m_oEndOfExpression) || (*m_iterPos)->IsOperator(p_par_open) == false)
		return failure;

	auto& oldIter = m_iterPos;

	std::advance(m_iterPos, 1); // skip (

	if (IsEndOfBuffer())
		return failure;

	if ((*m_iterPos)->IsOperator(p_par_close) == false) {
		if (!ParseParameters())
			return failure;
	} else {
		std::advance(m_iterPos, 1); // skip )
	}

	if (!IsArrowFunction())
		return failure; // just parentheses with a list of identifiers... ?

	std::advance(m_iterPos, 2); //skip =>

	CFunctionLinter fnLinter(m_iterPos, m_iterEnd, m_pScope, m_pOwner);

	auto& newScope = fnLinter.m_pThisScope;
	auto& newStack = fnLinter.m_pThisStack;

	fnLinter.m_oFunctionName = "arrow";
	fnLinter.m_oParameters = m_oParameters;

	for (auto& param : m_oParameters) {
		auto var = newStack->m_VariableManager->DeclareVariable(param);
		var->m_bParameter = true;

		if (!newScope->DeclareVariable(param)) {
			CLinterErrors::PushError("well then", (*m_iterPos)->m_oSourcePosition);
			return failure;
		}
	}

	newScope->MakeLoopScope(false);
	newStack->m_pLowerFunction = m_pOwner->IsGlobalMemory()
		? nullptr : m_pOwner->ToStack()->GetGlobalFunction();

	if (IsOneLiner()) {

		CLinterExpression linter(m_iterPos, m_iterEnd, newScope, newStack.get());

		if (!linter.Parse(m_oEndOfExpression)) {
			return failure;
		}

		newScope->AddInstruction(std::make_unique<CRuntimeReturnStatement>(linter.ToMergedAST()));
		if(m_oEndOfExpression)
			std::advance(m_iterPos, -1); // go back one token

	} else {
		if (!fnLinter.ParseFunctionScope()) {
			return failure;
		}
		std::advance(m_iterPos, 1); // skip }
	}

	fnLinter.m_pThisStack->m_pFunction = fnLinter.ToFunction();

	VectorOf<CCrossModuleReference> refs = m_pOwner->IsStack()
		? fnLinter.GetSharedOwnershipVariables(m_pOwner->ToStack())
		: VectorOf<CCrossModuleReference>{};

	m_pOperand = std::make_unique<CLambdaOperand>(fnLinter.ToRuntimeFunction(), std::move(refs));
	m_pOperand->m_oCodePosition = (*oldIter)->m_oSourcePosition;

	return success;

}
Success CLambdaChecker::ParseParameters()
{

	if (IsEndOfBuffer() || !CFunctionLinter::IsIdentifier((*m_iterPos))) {
		return failure; //not an identifier, can't be a parameter
	}

	auto scope = m_pScope.lock();

	if (!scope)
		return failure;

	const auto& identifier = (*m_iterPos)->Source();

	if (scope->VariableExists(identifier)) // already declared
		return failure;

	if (std::ranges::find(m_oParameters, identifier) != m_oParameters.end()) // already declared
		return failure;

	m_oParameters.push_back(identifier);

	std::advance(m_iterPos, 1); //skip identifier

	if (IsEndOfBuffer()) {
		return failure; //that's weird
	}

	if ((*m_iterPos)->IsOperator(p_comma)) {
		std::advance(m_iterPos, 1); // skip the comma
		return ParseParameters();
	}
	else if ((*m_iterPos)->IsOperator(p_par_close)) {
		std::advance(m_iterPos, 1); // skip the )
		return success;
	}

	return failure; //not a function parameter but an expression instead

}

bool CLambdaChecker::IsArrowFunction() const noexcept
{
	if (std::distance(m_iterPos, m_iterEnd) < 2)
		return false;

	return (*m_iterPos)->IsOperator(p_assign) && (*std::next(m_iterPos))->IsOperator(p_greater_than);
}
bool CLambdaChecker::IsOneLiner() const noexcept
{
	return !IsEndOfBuffer() && (*m_iterPos)->IsOperator(p_curlybracket_open) == false;
}
bool CLambdaChecker::EndOfExpression(const std::optional<PairMatcher>& eoe) const noexcept
{
	assert(m_iterPos != m_iterEnd);

	if (!eoe)
		return (*m_iterPos)->IsOperator(p_semicolon);

	if (!(*m_iterPos)->IsOperator())
		return false;

	return eoe->IsClosing(dynamic_cast<const CPunctuationToken*>(*m_iterPos)->m_ePunctuation);
}