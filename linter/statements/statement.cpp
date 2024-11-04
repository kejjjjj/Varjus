
#include "statement.hpp"
#include "linter/linter.hpp"
#include "linter/token.hpp"
#include "linter/error.hpp"
#include "linter/expressions/expression.hpp"
#include "linter/expressions/ast.hpp"
#include "linter/scopes/scope.hpp"

#include <cassert>


CStatementLinter::CStatementLinter(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const stack)
	: CLinterSingle(pos, end), m_pScope(scope), m_pOwner(stack) {

	assert(m_iterPos != m_iterEnd);

	if (const auto s = m_pScope.lock()) {
		m_pThisScope = s->CreateScope();
	} else {
		CLinterErrors::PushError("!(const auto scope = m_pScope.lock())", (*m_iterPos)->m_oSourcePosition);
		return;
	}
}

Success CStatementLinter::ParseIdentifier(TokenType tt)
{
	if (IsEndOfBuffer() || (*m_iterPos)->Type() != tt) {
		CLinterErrors::PushError("expected a statement", IsEndOfBuffer() ? (*std::prev(m_iterPos))->m_oSourcePosition : (*m_iterPos)->m_oSourcePosition);
		return failure;
	}

	std::advance(m_iterPos, 1); 

	return success;
}

std::unique_ptr<AbstractSyntaxTree> CStatementLinter::ParseExpression()
{
	if (IsEndOfBuffer() || !(*m_iterPos)->IsOperator(p_par_open)) {
		CLinterErrors::PushError("expected a \"(\"", IsEndOfBuffer() ? (*std::prev(m_iterPos))->m_oSourcePosition : (*m_iterPos)->m_oSourcePosition);
		return nullptr;
	}

	std::advance(m_iterPos, 1); // skip (

	CLinterExpression expr(m_iterPos, m_iterEnd, m_pScope, m_pOwner);

	std::unique_ptr<AbstractSyntaxTree>&& temp = nullptr;

	if (expr.ParseExpression(PairMatcher(p_par_open))) {
		temp = expr.ToMergedAST();
	}

	return temp;
}


Success CStatementLinter::ParseScope()
{
	if (IsEndOfBuffer()) {
		CLinterErrors::PushError("expected a \"{\" or an expression", (*std::prev(m_iterPos))->m_oSourcePosition);
		return failure;
	}


	// no curlybracket? only add one instruction :)
	if (!(*m_iterPos)->IsOperator(p_curlybracket_open)) {
		return CFileLinter::LintToken(m_iterPos, m_iterEnd, m_pThisScope, m_pOwner);
	}

	auto scope = CScopeLinter(m_iterPos, m_iterEnd, m_pThisScope, m_pOwner);
	if (!scope.ParseScope()) {
		return failure;
	}

	return success;
}