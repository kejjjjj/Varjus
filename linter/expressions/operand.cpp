#include "operand.hpp"
#include "unary.hpp"
#include "identifier.hpp"
#include "postfix.hpp"
#include "expression.hpp"

#include "linter/punctuation.hpp"
#include "linter/token.hpp"
#include "linter/error.hpp"
#include "linter/declarations/stack.hpp"

#include "ast.hpp"

#include "globalEnums.hpp"

#include <cassert>
#include <ranges>

CIdentifierLinter* COperandBase::GetIdentifier()const  noexcept {
	return dynamic_cast<const CIdentifierOperand*>(this)->m_oIdentifierToken.get();
}


CLinterOperand::CLinterOperand(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const stack)
	:CLinterSingle(pos, end), m_pScope(scope), m_pOwner(stack) {

	assert(m_iterPos != m_iterEnd);

}
CLinterOperand::~CLinterOperand() = default;

Success CLinterOperand::ParseOperand()
{
	// Parse unary
	CUnaryLinter unaryLinter(m_iterPos, m_iterEnd);
	if (!unaryLinter.ParseUnary())
		return failure;


	const auto token = (*m_iterPos);
	if (token->IsOperator(p_par_open)) {
		std::advance(m_iterPos, 1);

		CLinterExpression expr(m_iterPos, m_iterEnd, m_pScope, m_pOwner);		
		if (expr.ParseExpression(PairMatcher(p_par_open))) {
			m_pOperand = std::make_unique<CASTOperand>(expr.ToAST());
		}

	} else {
		auto operand = std::make_unique<CIdentifierOperand>(m_iterPos, m_iterEnd, m_pScope, m_pOwner);
		if (!operand->m_oIdentifierToken->ParseIdentifier()) {
			return failure;
		}

		m_pOperand = std::move(operand);
	}

	CPostfixLinter postfix(m_iterPos, m_iterEnd, m_pScope, m_pOwner);
	if (!postfix.ParsePostfix())
		return failure;

	// Save results
	m_oUnaryTokens = unaryLinter.GetTokens();
	m_oPostfixes = postfix.Move();
	return success;
}
bool CLinterOperand::IsExpression() const noexcept
{
	assert(m_pOperand != nullptr);
	return m_pOperand->Type() == abstract_syntax_tree;
}
std::unique_ptr<AbstractSyntaxTree> CLinterOperand::OperandToAST() const noexcept
{
	if (IsImmediate()) {
		const auto identifier = m_pOperand->GetIdentifier();
		return std::make_unique<ConstantASTNode>(identifier->ToData(), identifier->GetImmediateType());
	}
	else if (IsVariable()) {
		return std::make_unique<VariableASTNode>(GetVariable()->m_uIndex);
	}
	else if (IsExpression()) {
		return ExpressionToAST();
	}

	return nullptr;
}
std::unique_ptr<AbstractSyntaxTree> CLinterOperand::ExpressionToAST() const noexcept
{
	assert(m_pOperand != nullptr);
	assert(IsExpression());

	auto ast = dynamic_cast<CASTOperand*>(m_pOperand.get());
	return std::move(ast->m_pAST);
}

[[nodiscard]] std::unique_ptr<AbstractSyntaxTree> CLinterOperand::ToAST()
{
	if (auto pfs = PostfixesToAST()) {
		pfs->left = OperandToAST();
		return pfs;
	}

	return OperandToAST();
}

std::unique_ptr<AbstractSyntaxTree> CLinterOperand::PostfixesToAST() const noexcept
{
	if (!m_oPostfixes.size())
		return nullptr;

	std::unique_ptr<AbstractSyntaxTree> root;
	std::shared_ptr<AbstractSyntaxTree>* position{ nullptr };

	for (auto& pf : m_oPostfixes) {
		
		if (!root) {
			root = pf->ToAST();
			position = &root->left;
			continue;
		}

		assert(position);

		*position = pf->ToAST();
		position = &(*position)->left;
	}

	return root;
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

	const auto type = m_pOperand->GetIdentifier()->GetToken()->Type();
	return type >= tt_false && type <= tt_string;
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

	result += m_pOperand->GetIdentifier()->GetToken()->Source();
	return result;
}

/***********************************************************************
 > 
***********************************************************************/
CASTOperand::CASTOperand(std::unique_ptr<AbstractSyntaxTree>&& ptr) : m_pAST(std::move(ptr)) {}
CASTOperand::~CASTOperand() = default;




