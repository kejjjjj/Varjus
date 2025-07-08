#include "repeat.hpp"

#include "linter/token.hpp"
#include "linter/error.hpp"
#include "linter/punctuation.hpp"
#include "linter/expressions/expression.hpp"
#include "linter/expressions/ast.hpp"
#include "linter/scopes/scope.hpp"
#include "linter/modules/module.hpp"

#include "varjus_api/internal/globalDefinitions.hpp"

#include <cassert>
using namespace Varjus;

CRepeatStatementLinter::CRepeatStatementLinter(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const stack)
	: CStatementLinter(pos, end, scope, stack) {

	assert(m_iterPos != m_iterEnd);
}
CRepeatStatementLinter::~CRepeatStatementLinter() = default;

Varjus::Success CRepeatStatementLinter::Parse()
{
	m_pThisScope->MakeLoopScope();

	if (!ParseIdentifier(TokenType::tt_repeat))
		return failure;

	if (!ParseScope())
		return failure;

	std::advance(m_iterPos, 1); // skip }

	if (IsEndOfBuffer() || (*m_iterPos)->Type() != tt_while) {
		std::advance(m_iterPos, -1);
		return success;
	}

	std::advance(m_iterPos, 1); // skip while

	m_pCondition = ParseExpression();

	if (IsEndOfBuffer() || (*m_iterPos)->IsOperator(p_semicolon) == false) {
		m_pOwner->GetModule()->PushError(VSL("expected a \";\""), GetIteratorSafe()->m_oSourcePosition);
		return failure;
	}

	if (m_pOwner->IsHoisting())
		return success;

	return m_pCondition != nullptr ? success : failure;
}

RuntimeBlock CRepeatStatementLinter::ToRuntimeObject() const
{

	decltype(auto) tempAST = const_cast<ASTNode&&>(std::move(m_pCondition));
	return std::make_unique<CRuntimeRepeatStatement>(std::move(tempAST), m_pThisScope->MoveInstructions());
}