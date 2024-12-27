#include "operand.hpp"
#include "operand_expression.hpp"
#include "linter/expressions/expression.hpp"
#include "linter/expressions/ast.hpp"
#include "linter/token.hpp"

#include "operand_lambda.hpp"

std::unique_ptr<IOperand> CLinterOperand::ParseParentheses(std::optional<PairMatcher>& eoe)
{
	CLambdaChecker checker(m_iterPos, m_iterEnd, m_pScope, m_pOwner);

	if (checker.Parse(eoe)) {
		return checker.Result();
	}

	std::advance(m_iterPos, 1); // skip (

	const auto& oldIter = m_iterPos;

	CLinterExpression expr(m_iterPos, m_iterEnd, m_pScope, m_pOwner);
	if (!expr.Parse(PairMatcher(p_par_open)))
		return nullptr;

	auto&& ptr = std::make_unique<CASTOperand>(expr.ToMergedAST());
	ptr->m_oCodePosition = (*oldIter)->m_oSourcePosition;
	return ptr;
}

UniqueAST CASTOperand::ToAST(){
	return std::move(m_pAST);
}

CASTOperand::CASTOperand(std::unique_ptr<AbstractSyntaxTree>&& ptr) : m_pAST(std::move(ptr)) {}
CASTOperand::~CASTOperand() = default;