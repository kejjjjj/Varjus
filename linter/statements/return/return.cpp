#include "return.hpp"

#include "linter/token.hpp"
#include "linter/error.hpp"
#include "linter/punctuation.hpp"
#include "linter/expressions/expression.hpp"
#include "linter/expressions/ast.hpp"
#include "linter/scopes/scope.hpp"
#include "linter/modules/module.hpp"

#include "varjus_api/internal/globalDefinitions.hpp"

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
			m_pOwner->GetModule()->PushError("cannot return in the global scope", GetIteratorSafe()->m_oSourcePosition);
			return failure;
		}
	} else {
		m_pOwner->GetModule()->PushError("!(const auto scope = m_pScope.lock())", GetIteratorSafe()->m_oSourcePosition);
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
ASTNode CReturnStatementLinter::ParseExpression()
{

	CLinterExpression expr(m_iterPos, m_iterEnd, m_pScope, m_pOwner);

	if (expr.Parse()) {
		return expr.ToMergedAST();
	}

	return nullptr;
}

RuntimeBlock CReturnStatementLinter::ToRuntimeObject() const
{

	decltype(auto) tempAST = const_cast<ASTNode&&>(std::move(m_pAST));
	return std::make_unique<CRuntimeReturnStatement>(std::move(tempAST));
}