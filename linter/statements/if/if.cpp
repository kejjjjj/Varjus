#include "if.hpp"

#include "linter/token.hpp"
#include "linter/error.hpp"
#include "linter/punctuation.hpp"
#include "linter/expressions/expression.hpp"
#include "linter/expressions/ast.hpp"
#include "linter/scopes/scope.hpp"

#include "varjus_api/internal/globalDefinitions.hpp"

#include <cassert>

using namespace Varjus;

CIfStatementLinter::CIfStatementLinter(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const stack)
	: CStatementLinter(pos, end, scope, stack) {

	assert(m_iterPos != m_iterEnd);
}
CIfStatementLinter::~CIfStatementLinter() = default;

Varjus::Success CIfStatementLinter::Parse()
{

	if (!ParseIdentifier(TokenType::tt_if))
		return failure;

	m_pCondition = ParseExpression();
	return ParseScope();
}

__RuntimeBlock CIfStatementLinter::ToRuntimeObject() const
{
	decltype(auto) tempAST = const_cast<ASTNode&&>(std::move(m_pCondition));
	return tempAST ? std::make_unique<CRuntimeConditionalStatement>(std::move(tempAST), m_pThisScope->MoveInstructions()) : nullptr;
}
std::unique_ptr<CRuntimeConditionalStatement> CIfStatementLinter::ToConditionalObject()
{
	decltype(auto) tempAST = const_cast<ASTNode&&>(std::move(m_pCondition));
	return tempAST ? std::make_unique<CRuntimeConditionalStatement>(std::move(tempAST), m_pThisScope->MoveInstructions()) : nullptr;
}
