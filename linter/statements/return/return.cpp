#include "return.hpp"

#include "linter/token.hpp"
#include "linter/error.hpp"
#include "linter/punctuation.hpp"
#include "linter/expressions/expression.hpp"
#include "linter/expressions/ast.hpp"
#include "linter/scopes/scope.hpp"

#include "globalDefinitions.hpp"

#include <cassert>

CReturnStatementLinter::CReturnStatementLinter(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const stack)
	: CStatementLinter(pos, end, scope, stack) {

	assert(m_iterPos != m_iterEnd);
}
CReturnStatementLinter::~CReturnStatementLinter() = default;

Success CReturnStatementLinter::Parse()
{

	if (const auto scope = m_pScope.lock()) {
		if (scope->IsGlobalScope()) {
			CLinterErrors::PushError("cannot return in the global scope", IsEndOfBuffer() ? (*std::prev(m_iterPos))->m_oSourcePosition : (*m_iterPos)->m_oSourcePosition);
			return failure;
		}
	} else {
		CLinterErrors::PushError("!(const auto scope = m_pScope.lock())", IsEndOfBuffer() ? (*std::prev(m_iterPos))->m_oSourcePosition : (*m_iterPos)->m_oSourcePosition);
		return failure;
	}

	if (!ParseIdentifier(TokenType::tt_return))
		return failure;

	// return;
	if (!IsEndOfBuffer() && (*m_iterPos)->IsOperator(p_semicolon))
		return success;

	m_pAST = ParseExpression();
	return success;
}
std::unique_ptr<AbstractSyntaxTree> CReturnStatementLinter::ParseExpression()
{

	CLinterExpression expr(m_iterPos, m_iterEnd, m_pScope, m_pOwner);

	if (expr.Parse()) {
		return expr.ToMergedAST();
	}

	return nullptr;
}

RuntimeBlock CReturnStatementLinter::ToRuntimeObject() const
{

	decltype(auto) tempAST = const_cast<std::unique_ptr<AbstractSyntaxTree>&&>(std::move(m_pAST));
	return tempAST ? std::make_unique<CRuntimeReturnStatement>(std::move(tempAST)) : nullptr;
}