#include "while.hpp"

#include "linter/token.hpp"
#include "linter/error.hpp"
#include "linter/punctuation.hpp"
#include "linter/expressions/expression.hpp"
#include "linter/expressions/ast.hpp"
#include "linter/scopes/scope.hpp"

#include "varjus_api/internal/globalDefinitions.hpp"

#include <cassert>

using namespace Varjus;

CWhileStatementLinter::CWhileStatementLinter(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const stack)
	: CStatementLinter(pos, end, scope, stack) {

	assert(m_iterPos != m_iterEnd);
}
CWhileStatementLinter::~CWhileStatementLinter() = default;

Varjus::Success CWhileStatementLinter::Parse()
{
	m_pThisScope->MakeLoopScope();

	if (!ParseIdentifier(TokenType::tt_while))
		return failure;

	m_pCondition = ParseExpression();
	return ParseScope();
}

__RuntimeBlock CWhileStatementLinter::ToRuntimeObject() const
{
	decltype(auto) tempAST = const_cast<ASTNode&&>(std::move(m_pCondition));
	return tempAST ? std::make_unique<CRuntimeWhileStatement>(std::move(tempAST), m_pThisScope->MoveInstructions()) : nullptr;
}