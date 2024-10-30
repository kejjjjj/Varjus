
#include "statement.hpp"
#include "linter/token.hpp"
#include "linter/error.hpp"
#include "linter/expressions/expression.hpp"
#include "linter/expressions/ast.hpp"
#include "linter/scopes/scope.hpp"

#include <cassert>


CStatementParser::CStatementParser(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const stack)
	: CLinterSingle(pos, end), m_pScope(scope), m_pOwner(stack) {

	assert(m_iterPos != m_iterEnd);

	if (const auto s = m_pScope.lock()) {
		m_pThisScope = s->CreateScope();
	} else {
		CLinterErrors::PushError("!(const auto scope = m_pScope.lock())", (*m_iterPos)->m_oSourcePosition);
		return;
	}
}

Success CStatementParser::ParseIdentifier(TokenType tt)
{
	if (IsEndOfBuffer() || (*m_iterPos)->Type() != tt) {
		CLinterErrors::PushError("expected a statement", IsEndOfBuffer() ? (*std::prev(m_iterPos))->m_oSourcePosition : (*m_iterPos)->m_oSourcePosition);
		return failure;
	}

	std::advance(m_iterPos, 1); 

	return success;
}

std::unique_ptr<AbstractSyntaxTree> CStatementParser::ParseExpression()
{
	if (IsEndOfBuffer() || !(*m_iterPos)->IsOperator(p_par_open)) {
		CLinterErrors::PushError("expected a \"(\"", IsEndOfBuffer() ? (*std::prev(m_iterPos))->m_oSourcePosition : (*m_iterPos)->m_oSourcePosition);
		return nullptr;
	}

	std::advance(m_iterPos, 1); // skip (

	CLinterExpression expr(m_iterPos, m_iterEnd, m_pScope, m_pOwner);

	std::unique_ptr<AbstractSyntaxTree>&& temp = nullptr;

	if (expr.ParseExpression(PairMatcher(p_par_open))) {
		temp = expr.ToAST();
	}

	//if (IsEndOfBuffer() || !(*m_iterPos)->IsOperator(p_par_close)) {
	//	CLinterErrors::PushError("expected a \")\"", IsEndOfBuffer() ? (*std::prev(m_iterPos))->m_oSourcePosition : (*m_iterPos)->m_oSourcePosition);
	//	return nullptr;
	//}

	//std::advance(m_iterPos, 1); // skip )
	return temp;
}


Success CStatementParser::ParseScope()
{
	if (IsEndOfBuffer() || !(*m_iterPos)->IsOperator(p_curlybracket_open)) {
		CLinterErrors::PushError("expected a \"{\"", IsEndOfBuffer() ? (*std::prev(m_iterPos))->m_oSourcePosition : (*m_iterPos)->m_oSourcePosition);
		return failure;
	}

	auto scope = CScopeLinter(m_iterPos, m_iterEnd, m_pThisScope, m_pOwner);
	if (!scope.ParseScope()) {
		return failure;
	}

	return success;
}