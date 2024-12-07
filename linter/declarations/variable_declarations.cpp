#include "variable_declarations.hpp"
#include "linter/functions/stack.hpp"
#include "linter/token.hpp"
#include "linter/error.hpp"
#include "linter/scopes/scope.hpp"
#include "linter/punctuation.hpp"
#include "linter/expressions/expression.hpp"
#include "linter/expressions/ast.hpp"

#include <cassert>

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

	std::advance(m_iterPos, 1);


	if (IsEndOfBuffer() ||!IsIdentifier(*m_iterPos)) {
		CLinterErrors::PushError("expected variable name, but found " + 
			GetIteratorSafe()->Source(), GetIteratorSafe()->m_oSourcePosition);
		return failure;
	}

	if (const auto scope = m_pScope.lock()) {

		if (!scope->DeclareVariable((*m_iterPos)->Source())) {
			CLinterErrors::PushError("variable " + (*m_iterPos)->Source() + " already declared", (*m_iterPos)->m_oSourcePosition);
			return failure;
		}

		if(scope->IsGlobalScope())
			m_pOwner->GetGlobalMemory()->m_VariableManager->DeclareVariable((*m_iterPos)->Source());
		else
			m_pOwner->m_VariableManager->DeclareVariable((*m_iterPos)->Source());

	} else {
		CLinterErrors::PushError("!(const auto scope = currentScope.lock())", (*m_iterPos)->m_oSourcePosition);
		return failure;
	}

	std::advance(m_iterPos, 1);

	if (IsEndOfBuffer()) {
		CLinterErrors::PushError("expected \";\"", GetIteratorSafe()->m_oSourcePosition);
		return failure;
	}

	//let var;
	if ((*m_iterPos)->IsOperator(p_semicolon))
		return success;

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

	m_pInitializerAST = linter.ToMergedAST();
	return success;
}


bool CVariableDeclarationLinter::IsDeclaration(const CToken* token) const noexcept
{
	return token && token->Type() == tt_declaration;
}
bool CVariableDeclarationLinter::IsIdentifier(const CToken* token) const noexcept
{
	return token && token->Type() == tt_name;
}

bool CVariableDeclarationLinter::IsGlobalVariable() const noexcept
{
	assert(m_pOwner);
	return !m_pOwner->IsStack();
}

RuntimeBlock CVariableDeclarationLinter::ToRuntimeObject() const
{
	// yes this is very not at all undefined behavior :pagman:
	decltype(auto) tempAST = const_cast<std::unique_ptr<AbstractSyntaxTree>&&>(std::move(m_pInitializerAST));
	return tempAST ? std::make_unique<CRuntimeExpression>(std::move(tempAST)) : nullptr;
}
std::unique_ptr<AbstractSyntaxTree>&& CVariableDeclarationLinter::MoveInitializer() {
	return std::move(m_pInitializerAST);
}