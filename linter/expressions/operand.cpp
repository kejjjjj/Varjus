#include "operand.hpp"
#include "unary.hpp"
#include "identifier.hpp"

#include "linter/punctuation.hpp"
#include "linter/token.hpp"
#include "linter/error.hpp"
#include "linter/declarations/stack.hpp"
#include "globalEnums.hpp"

#include "ast.hpp"

#include <cassert>
#include "expression.hpp"
#include <iostream>

CIdentifierLinter* COperandBase::GetIdentifier()const  noexcept {
	return dynamic_cast<const CIdentifierOperand*>(this)->m_oIdentifierToken.get();
}


CLinterOperand::CLinterOperand(LinterIterator& pos, LinterIterator& end, CMemory* const stack) 
	: m_iterPos(pos), m_iterEnd(end), m_pOwner(stack) {

	assert(m_iterPos != m_iterEnd);

}
CLinterOperand::~CLinterOperand() = default;

Success CLinterOperand::ParseOperand()
{
	// Parse unary
	CUnaryLinter unaryLinter(m_iterPos, m_iterEnd);
	unaryLinter.ParseUnary();


	const auto token = (*m_iterPos);
	if (token->IsOperator(p_par_open)) {
		std::advance(m_iterPos, 1);

		CLinterExpression expr(m_iterPos, m_iterEnd, m_pOwner);		
		if (expr.ParseExpression(PairMatcher(p_par_open))) {
			m_pOperand = std::make_unique<CASTOperand>(expr.ToAST());
		}

	} else {
		auto operand = std::make_unique<CIdentifierOperand>(m_iterPos, m_iterEnd, m_pOwner);
		if (!operand->m_oIdentifierToken->ParseIdentifier()) {
			return failure;
		}

		m_pOperand = std::move(operand);
	}

	//TODO: Parse postfix

	// Save results
	m_oUnaryTokens = unaryLinter.GetResult();

	return success;
}
bool CLinterOperand::IsExpression() const noexcept
{
	assert(m_pOperand != nullptr);
	return m_pOperand->Type() == abstract_syntax_tree;
}
std::unique_ptr<AbstractSyntaxTree> CLinterOperand::ExpressionToAST() const noexcept
{
	assert(m_pOperand != nullptr);
	assert(IsExpression());

	auto ast = dynamic_cast<CASTOperand*>(m_pOperand.get());
	return std::move(ast->m_pAST);
}
bool CLinterOperand::IsVariable() const noexcept
{
	assert(m_pOperand != nullptr);

	if (IsExpression())
		return false;

	return m_pOperand->GetIdentifier()->m_pVariable != nullptr;
}
bool CLinterOperand::IsImmediate() const noexcept
{
	assert(m_pOperand != nullptr);

	if (IsExpression())
		return false;

	const auto type = m_pOperand->GetIdentifier()->GetResult()->Type();
	return type == t_int || type == t_double;
}
const CLinterVariable* CLinterOperand::GetVariable() const noexcept
{
	assert(m_pOperand != nullptr);
	assert(!IsExpression());

	return m_pOperand->GetIdentifier()->m_pVariable;
}
std::string CLinterOperand::ToString() const noexcept
{
	assert(m_pOperand != nullptr);
	assert(!IsExpression());

	std::string result;
	for (const auto& unary : m_oUnaryTokens) {
		result += unary->Source();
	}

	result += m_pOperand->GetIdentifier()->GetResult()->Source();
	return result;
}

/***********************************************************************
 > 
***********************************************************************/
CASTOperand::CASTOperand(std::unique_ptr<AbstractSyntaxTree>&& ptr) : m_pAST(std::move(ptr)) {}
CASTOperand::~CASTOperand() = default;




