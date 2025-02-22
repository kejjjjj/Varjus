#include "throw.hpp"

#include "linter/token.hpp"
#include "linter/error.hpp"
#include "linter/punctuation.hpp"
#include "linter/expressions/expression.hpp"
#include "linter/expressions/ast.hpp"
#include "linter/scopes/scope.hpp"

#include "api/internal/globalDefinitions.hpp"

#include <cassert>

CThrowStatementLinter::CThrowStatementLinter(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const stack)
	: CStatementLinter(pos, end, scope, stack) {

	assert(m_iterPos != m_iterEnd);
}
CThrowStatementLinter::~CThrowStatementLinter() = default;

Success CThrowStatementLinter::Parse()
{
	if (!ParseIdentifier(TokenType::tt_throw))
		return failure;

	// throw;
	if (!IsEndOfBuffer() && (*m_iterPos)->IsOperator(p_semicolon)) {
		CLinterErrors::PushError("expected an expression", GetIteratorSafe()->m_oSourcePosition);
		return success;
	}

	m_pAST = ParseExpression();
	return success;
}
ASTNode CThrowStatementLinter::ParseExpression()
{

	CLinterExpression expr(m_iterPos, m_iterEnd, m_pScope, m_pOwner);

	if (expr.Parse()) {
		return expr.ToMergedAST();
	}

	return nullptr;
}

RuntimeBlock CThrowStatementLinter::ToRuntimeObject() const
{
	decltype(auto) tempAST = const_cast<ASTNode&&>(std::move(m_pAST));
	return std::make_unique<CRuntimeThrowStatement>(std::move(tempAST));
}