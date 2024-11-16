#include "operand.hpp"
#include "operand_expression.hpp"
#include "linter/expressions/expression.hpp"
#include "linter/expressions/ast.hpp"

#include "linter/token.hpp"

std::unique_ptr<IOperand> CLinterOperand::ParseParentheses()
{
	std::advance(m_iterPos, 1); // skip (

	CLinterExpression expr(m_iterPos, m_iterEnd, m_pScope, m_pOwner);
	if (!expr.Parse(PairMatcher(p_par_open)))
		return nullptr;

	return std::make_unique<CASTOperand>(expr.ToMergedAST());
}

UniqueAST CASTOperand::ToAST(){
	return std::move(m_pAST);
}

CASTOperand::CASTOperand(std::unique_ptr<AbstractSyntaxTree>&& ptr) : m_pAST(std::move(ptr)) {}
CASTOperand::~CASTOperand() = default;