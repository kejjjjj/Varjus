#include "case.hpp"

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

CCaseStatementLinter::CCaseStatementLinter(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const stack)
	: CStatementLinter(pos, end, scope, stack) {

	assert(m_iterPos != m_iterEnd);
}
CCaseStatementLinter::~CCaseStatementLinter() = default;

Varjus::Success CCaseStatementLinter::Parse()
{
	//m_pThisScope->MakeLoopScope();

	if (IsEndOfBuffer() || ((*m_iterPos)->Type() != TokenType::tt_case && (*m_iterPos)->Type() != TokenType::tt_default)) {
		m_pOwner->GetModule()->PushError(Varjus::fmt::format(VSL("expected \"case\" or \"default\"")), GetIteratorSafe()->m_oSourcePosition);
		return failure;
	}

	if ((*m_iterPos)->Type() == TokenType::tt_default) {
		return ParseDefaultClause();
	}

	std::advance(m_iterPos, 1);

	CLinterExpression expr(m_iterPos, m_iterEnd, m_pScope, m_pOwner);
	if (!expr.Parse(PairMatcher(p_colon))) {
		return failure;
	}

	m_pExpression = expr.ToMergedAST();
	return ParseCaseScope();

}
Varjus::Success CCaseStatementLinter::ParseCaseScope()
{
	CScopeLinter sc(m_iterPos, m_iterEnd, m_pThisScope, m_pOwner);

	if (!sc.ParseUntil([](LinterIterator& i) {
		return (*i)->Type() == tt_case || (*i)->Type() == tt_default || (*i)->IsOperator(p_curlybracket_close); })) {
		return failure;
	}

	if (IsEndOfBuffer())
		return failure;

	return success;

}
Varjus::Success CCaseStatementLinter::ParseDefaultClause()
{

	if (ContainsDefaultClause()) {
		m_pOwner->GetModule()->PushError(Varjus::fmt::format(VSL("\"default\" clause already defined")), GetIteratorSafe()->m_oSourcePosition);
		return failure;
	}

	std::advance(m_iterPos, 1);

	if (IsEndOfBuffer() || !(*m_iterPos)->IsOperator(p_colon)) {
		m_pOwner->GetModule()->PushError(Varjus::fmt::format(VSL("expected \":\"")), GetIteratorSafe()->m_oSourcePosition);
		return failure;
	}

	std::advance(m_iterPos, 1);
	return ParseCaseScope();
}

bool CCaseStatementLinter::ContainsDefaultClause() const noexcept
{

	const auto s = m_pScope.lock();

	assert(s);

	if (!s)
		return false;
		
	auto& instructions = s->GetInstructions();

	//a match statement should ONLY contain cases
	for (const auto& insn : instructions) {
		assert(insn->Type() == st_case);

		auto asCase = insn->ToCaseStatement();
		assert(asCase);

		if (asCase && asCase->IsDefaultClause())
			return true;

	}

	return false;
}

__RuntimeBlock CCaseStatementLinter::ToRuntimeObject() const
{
	decltype(auto) tempAST = const_cast<ASTNode&&>(std::move(m_pExpression));
	return std::make_unique<CRuntimeCaseStatement>(std::move(tempAST), m_pThisScope->MoveInstructions());
}