#include "match.hpp"

#include "linter/token.hpp"
#include "linter/error.hpp"
#include "linter/punctuation.hpp"
#include "linter/expressions/expression.hpp"
#include "linter/expressions/ast.hpp"
#include "linter/scopes/scope.hpp"
#include "linter/modules/module.hpp"

#include "varjus_api/internal/globalDefinitions.hpp"

#include <cassert>

CMatchStatementLinter::CMatchStatementLinter(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const stack)
	: CStatementLinter(pos, end, scope, stack) {

	assert(m_iterPos != m_iterEnd);
}
CMatchStatementLinter::~CMatchStatementLinter() = default;

Success CMatchStatementLinter::Parse()
{

	if (!ParseIdentifier(TokenType::tt_match))
		return failure;

	m_pThisScope->MakeMatchScope(false);
	m_pExpression = ParseExpression();
	m_pThisScope->MakeMatchScope();

	//switch statements without {} are very yucky
	if (IsEndOfBuffer() || !(*m_iterPos)->IsOperator(p_curlybracket_open)) {
		m_pOwner->GetModule()->PushError(VSL("expected a \"{\" or an expression"), GetIteratorSafe()->m_oSourcePosition);
		return failure;
	}

	return ParseScope();

}
CRuntimeCaseStatement* CMatchStatementLinter::GetDefaultCase() const noexcept {

	const auto& insns = m_pThisScope->GetInstructions();

	for (const auto& insn : insns) {
		assert(insn);
		const auto asCase = insn->ToCaseStatement();
		assert(asCase);

		if (asCase && asCase->IsDefaultClause())
			return asCase;
	}

	return nullptr;

}
RuntimeBlock CMatchStatementLinter::ToRuntimeObject() const
{
	decltype(auto) tempAST = const_cast<ASTNode&&>(std::move(m_pExpression));
	return tempAST ? std::make_unique<CRuntimeMatchStatement>(std::move(tempAST), GetDefaultCase(), m_pThisScope->MoveInstructions()) : nullptr;
}