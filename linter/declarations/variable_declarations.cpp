#include "variable_declarations.hpp"
#include "linter/functions/stack.hpp"
#include "linter/token.hpp"
#include "linter/error.hpp"
#include "linter/scopes/scope.hpp"
#include "linter/punctuation.hpp"
#include "linter/expressions/expression.hpp"
#include "linter/expressions/ast.hpp"

#include <cassert>
#include <format>

#ifdef OPTIMIZATIONS
#include "linter/optimizations/optimizations.hpp"
#endif

CVariableDeclarationLinter::CVariableDeclarationLinter(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const stack) :
	CLinterSingle(pos, end), m_pScope(scope), m_pOwner(stack)
{
	assert(m_pOwner != nullptr);
}
CVariableDeclarationLinter::~CVariableDeclarationLinter() = default;

Success CVariableDeclarationLinter::Parse()
{

	if (IsEndOfBuffer() || !IsDeclaration(*m_iterPos)) {
		CLinterErrors::PushError("expected \"let\"", GetIteratorSafe()->m_oSourcePosition);
		return failure;
	}

	const auto isConst = (*m_iterPos)->Type() == tt_const;

	std::advance(m_iterPos, 1);


	if (IsEndOfBuffer() || !IsIdentifier(*m_iterPos)) {
		CLinterErrors::PushError(std::format("expected variable name, but found \"{}\"", GetIteratorSafe()->Source()), 
			GetIteratorSafe()->m_oSourcePosition);
		return failure;
	}

	const auto& varName = (*m_iterPos)->Source();

	if (const auto scope = m_pScope.lock()) {

		const auto containsFunc = m_pOwner->GetGlobalMemory()->m_FunctionManager->ContainsFunction(varName);

		if (containsFunc || !scope->DeclareVariable((*m_iterPos)->Source())) {
			CLinterErrors::PushError(std::format("\"{}\" already declared", (*m_iterPos)->Source()), (*m_iterPos)->m_oSourcePosition);
			return failure;
		}
#ifdef OPTIMIZATIONS
		//blah blah too lazy don't want to implement
		//only optimize local variables
		//if(m_pOwner->IsStack())
		//	m_sDeclaredVariable = m_pOwner->m_ConstEvalVariableManager->DeclareVariable(varName);
#else
		m_sDeclaredVariable = m_pOwner->m_VariableManager->DeclareVariable(varName);
#endif
	} else {
		CLinterErrors::PushError("!(const auto scope = currentScope.lock())", (*m_iterPos)->m_oSourcePosition);
		return failure;
	}

	std::advance(m_iterPos, 1); // skip identifier

	if (IsEndOfBuffer()) {
		CLinterErrors::PushError("expected \";\"", GetIteratorSafe()->m_oSourcePosition);
		return failure;
	}

#ifdef OPTIMIZATIONS
	//start off with a simple consteval undefined
	m_sDeclaredVariable->m_pConstEval = COptimizationValues::AcquireNewVariable();
	m_sDeclaredVariable->m_pConstEval->SetValue(COptimizationValues::AcquireNewValue<IConstEvalValue>());
#endif

	//let var;
	if ((*m_iterPos)->IsOperator(p_semicolon)) {
		m_sDeclaredVariable->m_bConst = isConst;
		return success;
	}

	//let var = expression;
	if (!(*m_iterPos)->IsOperator(p_assign)) {
		CLinterErrors::PushError("expected \";\" or \"=\"", (*m_iterPos)->m_oSourcePosition);
		return failure;
	}

	//parse initializer
	std::advance(m_iterPos, -1);

	CLinterExpression linter(m_iterPos, m_iterEnd, m_pScope, m_pOwner);
	if (!linter.Parse())
		return failure;

	m_sDeclaredVariable->m_bConst = isConst;

	m_pInitializerAST = linter.ToMergedAST();
	return success;
}
bool CVariableDeclarationLinter::IsDeclaration(const CToken* token) noexcept{
	return token && (token->Type() == tt_let || token->Type() == tt_const);
}
bool CVariableDeclarationLinter::IsIdentifier(const CToken* token) const noexcept{
	return token && token->Type() == tt_name;
}
RuntimeBlock CVariableDeclarationLinter::ToRuntimeObject() const
{
	// yes this is very not at all undefined behavior :pagman:
	decltype(auto) tempAST = const_cast<ASTNode&&>(std::move(m_pInitializerAST));
	return tempAST ? std::make_unique<CRuntimeExpression>(std::move(tempAST)) : nullptr;
}
ASTNode&& CVariableDeclarationLinter::MoveInitializer() {
	return std::move(m_pInitializerAST);
}