#include "operand.hpp"
#include "operand_array.hpp"
#include "linter/expressions/expression.hpp"
#include "linter/expressions/ast.hpp"

#include "linter/token.hpp"

std::unique_ptr<IOperand> CLinterOperand::ParseArray()
{
	std::advance(m_iterPos, 1); // skip [

	if (IsEndOfBuffer())
		return std::make_unique<CArrayOperand>();

	if ((*m_iterPos)->IsOperator(p_bracket_close)) {
		std::advance(m_iterPos, 1); // skip ]
		return std::make_unique<CArrayOperand>();
	}

	auto& pos  = (*m_iterPos)->m_oSourcePosition;

	CLinterExpression expr(m_iterPos, m_iterEnd, m_pScope, m_pOwner);
	if (!expr.Parse(PairMatcher(p_bracket_open)))
		return nullptr;

	auto&& ptr = std::make_unique<CArrayOperand>(expr.ToExpressionList());
	ptr->m_oCodePosition = pos;
	return ptr;
}

ASTNode CArrayOperand::ToAST() {
	return std::make_shared<ArrayASTNode>(m_oCodePosition, std::move(m_oExpressions));
}

CArrayOperand::CArrayOperand(__ExpressionList&& ptr) : m_oExpressions(std::move(ptr)) {}
CArrayOperand::~CArrayOperand() = default;