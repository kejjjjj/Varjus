#include "operand.hpp"
#include "operand_array.hpp"
#include "linter/expressions/expression.hpp"
#include "linter/expressions/ast.hpp"

#include "linter/token.hpp"

std::unique_ptr<IOperand> CLinterOperand::ParseArray()
{
	std::advance(m_iterPos, 1); // skip [

	if (IsEndOfBuffer() || (*m_iterPos)->IsOperator(p_bracket_close))
		return std::make_unique<CArrayOperand>();

	CLinterExpression expr(m_iterPos, m_iterEnd, m_pScope, m_pOwner);
	if (!expr.Parse(PairMatcher(p_bracket_open)))
		return nullptr;

	return std::make_unique<CArrayOperand>(expr.ToExpressionList());
}

UniqueAST CArrayOperand::ToAST() {
	return std::make_unique<ArrayASTNode>(std::move(m_oExpressions));
}

CArrayOperand::CArrayOperand(ExpressionList&& ptr) : m_oExpressions(std::move(ptr)) {}
CArrayOperand::~CArrayOperand() = default;