#include "operand.hpp"
#include "operand_ternary.hpp"
#include "linter/expressions/ast.hpp"
#include "linter/expressions/expression.hpp"
#include "linter/functions/stack.hpp"
#include "linter/error.hpp"
#include "linter/token.hpp"
#include <cassert>

std::unique_ptr<IOperand> CLinterOperand::ParseTernary(std::optional<PairMatcher>& eoe)
{
	assert((*m_iterPos)->IsOperator(p_question_mark));
	auto& oldIter = m_iterPos;
	std::advance(m_iterPos, 1); // skip ?

	auto ifTrue = CLinterExpression(m_iterPos, m_iterEnd, m_pScope, m_pOwner);
	if (!ifTrue.Parse(PairMatcher(p_colon))) // the expression can't end with eoe context, because we also need the : part
		return nullptr;

	if (IsEndOfBuffer() || !(*std::prev(m_iterPos))->IsOperator(p_colon)) {
		CLinterErrors::PushError("expected \":\", but found " + (*std::prev(m_iterPos))->Source(),
			GetIteratorSafe()->m_oSourcePosition);
		return nullptr;
	}

	auto ifFalse = CLinterExpression(m_iterPos, m_iterEnd, m_pScope, m_pOwner);

	const auto hadEoe = eoe != std::nullopt;

	if (!ifFalse.Parse(eoe))
		return nullptr;

	if (hadEoe)
		std::advance(m_iterPos, -1);

	if (m_pOwner->IsHoisting())
		return nullptr;

	auto&& ptr = std::make_unique<CTernaryOperand>(ToAST(), ifTrue.ToMergedAST(), ifFalse.ToMergedAST());
	ptr->m_oCodePosition = (*oldIter)->m_oSourcePosition;
	return ptr;
}

UniqueAST CTernaryOperand::ToAST() {
	return std::make_unique<TernaryASTNode>(m_oCodePosition, this);
}

CTernaryOperand::CTernaryOperand(UniqueAST&& value, UniqueAST&& m_true, UniqueAST&& m_false)
	: m_pValue(std::move(value)), m_pTrue(std::move(m_true)), m_pFalse(std::move(m_false)) {
}
CTernaryOperand::~CTernaryOperand() = default;