#include "variable_declarations.hpp"
#include "stack.hpp"
#include "linter/token.hpp"
#include "linter/error.hpp"
#include "linter/linter.hpp"
#include "linter/scopes/scope.hpp"
#include "linter/punctuation.hpp"
#include "linter/expressions/expression.hpp"

#include <cassert>


CVariableDeclaration::CVariableDeclaration(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const stack) :
	CLinterSingle(pos, end), m_pScope(scope), m_pOwner(stack)
{
	assert(m_pOwner != nullptr);
}


Success CVariableDeclaration::ParseDeclaration()
{
	if (IsEndOfBuffer()) {
		CLinterErrors::PushError("expected \"let\", but reached end of file", (*std::prev(m_iterPos))->m_oSourcePosition);
		return failure;
	}

	if (!IsDeclaration(*m_iterPos)) {
		CLinterErrors::PushError("expected \"let\"", (*m_iterPos)->m_oSourcePosition);
		return failure;
	}

	std::advance(m_iterPos, 1);

	if (IsEndOfBuffer()) {
		CLinterErrors::PushError("expected variable name, but reached end of file", (*std::prev(m_iterPos))->m_oSourcePosition);
		return failure;
	}

	if (!IsIdentifier(*m_iterPos)) {
		CLinterErrors::PushError("expected variable name, but found " + (*m_iterPos)->Source(), (*m_iterPos)->m_oSourcePosition);
		return failure;
	}

	if (const auto scope = m_pScope.lock()) {

		if (!scope->DeclareVariable((*m_iterPos)->Source())) {
			CLinterErrors::PushError("variable " + (*m_iterPos)->Source() + " already declared", (*m_iterPos)->m_oSourcePosition);
			return failure;
		}

		m_pOwner->DeclareVariable((*m_iterPos)->Source());
	} else {
		CLinterErrors::PushError("!(const auto scope = currentScope.lock())", (*m_iterPos)->m_oSourcePosition);
		return failure;
	}

	std::advance(m_iterPos, 1);

	if (IsEndOfBuffer()) {
		CLinterErrors::PushError("expected \";\"", (*std::prev(m_iterPos))->m_oSourcePosition);
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
	CFileLinter::LintExpression(m_iterPos, m_iterEnd, m_pScope, m_pOwner);
	return success;
}


bool CVariableDeclaration::IsDeclaration(const CToken* token) const noexcept
{
	return token && token->Type() == t_declaration;
}
bool CVariableDeclaration::IsIdentifier(const CToken* token) const noexcept
{
	return token && token->Type() == t_name;
}

bool CVariableDeclaration::IsGlobalVariable() const noexcept
{
	assert(m_pOwner);
	return !m_pOwner->IsStack();
}
