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
#include <iostream>

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
		m_pOperand = ParseParentheses();
	} else if (token->IsOperator(p_bracket_open)) {
		m_pOperand = ParseArray();
	} else {
		m_pOperand = ParseIdentifier();
	}

	CPostfixLinter postfix(m_iterPos, m_iterEnd, m_pScope, m_pOwner);
	if (!postfix.ParsePostfix())
		return failure;

	// Save results
	m_oUnaryTokens = unaryLinter.GetTokens();
	m_oPostfixes = postfix.Move();
	return success;
}
std::unique_ptr<COperandBase> CLinterOperand::ParseParentheses()
{
	std::advance(m_iterPos, 1); // skip (

	CLinterExpression expr(m_iterPos, m_iterEnd, m_pScope, m_pOwner);
	if (!expr.Parse(PairMatcher(p_par_open))) 
		return nullptr;

	return std::make_unique<CASTOperand>(expr.ToMergedAST());
}
std::unique_ptr<COperandBase> CLinterOperand::ParseIdentifier()
{
	auto operand = std::make_unique<CIdentifierOperand>(m_iterPos, m_iterEnd, m_pScope, m_pOwner);
	if (!operand->m_oIdentifierToken->ParseIdentifier()) {
		return nullptr;
	}

	return operand;
}
std::unique_ptr<COperandBase> CLinterOperand::ParseArray()
{
	std::advance(m_iterPos, 1); // skip [

	if(IsEndOfBuffer() || (*m_iterPos)->IsOperator(p_bracket_close))
		return std::make_unique<CArrayOperand>();

	CLinterExpression expr(m_iterPos, m_iterEnd, m_pScope, m_pOwner);
	if (!expr.Parse(PairMatcher(p_bracket_open)))
		return nullptr;

	return std::make_unique<CArrayOperand>(expr.ToExpressionList());
}

bool CLinterOperand::IsExpression() const noexcept
{
	assert(m_pOperand != nullptr);
	return m_pOperand->Type() == ot_abstract_syntax_tree;
}
bool CLinterOperand::IsArray() const noexcept
{
	assert(m_pOperand != nullptr);
	return m_pOperand->Type() == ot_array;
}
[[nodiscard]] CArrayOperand* CLinterOperand::GetArray() const noexcept
{
	assert(IsArray());
	return dynamic_cast<CArrayOperand*>(m_pOperand.get());
}
std::unique_ptr<AbstractSyntaxTree> CLinterOperand::OperandToAST() const noexcept
{
	if (IsImmediate()) {
		const auto identifier = m_pOperand->GetIdentifier();
		return std::make_unique<ConstantASTNode>(identifier->ToData(), identifier->GetImmediateType());
	} else if (IsVariable()) {
		return std::make_unique<VariableASTNode>(GetVariable()->m_uIndex);
	} else if (IsFunction()) {
		return std::make_unique<FunctionASTNode>(GetFunction()->m_uIndex);
	} else if (IsArray()) {
		return std::make_unique<ArrayASTNode>(std::move(GetArray()->m_oExpressions));
	} else if (IsExpression()) {
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
		auto end = pfs.get();
		while (end->left)
			end = end->left.get();

		end->left = OperandToAST();
		return pfs;
	}

	return OperandToAST();
}

std::unique_ptr<AbstractSyntaxTree> CLinterOperand::PostfixesToAST() const noexcept
{
	if (!m_oPostfixes.size())
		return nullptr;

	std::unique_ptr<AbstractSyntaxTree> root;
	AbstractSyntaxTree* position{ nullptr };

	for (auto& pf : m_oPostfixes) {
		
		if (!root) {
			root = pf->ToAST();
			position = root.get();
			continue;
		}

		assert(position);

		position->left = pf->ToAST();
		position = position->left.get();
	}

	return root;
}
bool CLinterOperand::IsImmediate() const noexcept
{
	assert(m_pOperand != nullptr);

	if (IsExpression() || IsArray())
		return false;

	const auto type = m_pOperand->GetIdentifier()->GetToken()->Type();
	return IS_IMMEDIATE(type);
}
bool CLinterOperand::IsVariable() const noexcept
{
	assert(m_pOperand != nullptr);

	if (IsExpression() || IsArray() || !m_pOperand->GetIdentifier()->m_pIdentifier)
		return false;

	return m_pOperand->GetIdentifier()->m_pIdentifier->Type() == mi_variable;
}
const CLinterVariable* CLinterOperand::GetVariable() const noexcept
{
	assert(IsVariable());
	return dynamic_cast<const CLinterVariable*>(m_pOperand->GetIdentifier()->m_pIdentifier);
}
bool CLinterOperand::IsFunction() const noexcept 
{
	assert(m_pOperand != nullptr);

	if (IsExpression() || IsArray() || !m_pOperand->GetIdentifier()->m_pIdentifier)
		return false;

	return m_pOperand->GetIdentifier()->m_pIdentifier->Type() == mi_function;
}
const CLinterFunction* CLinterOperand::GetFunction() const noexcept 
{
	assert(IsFunction());
	return dynamic_cast<const CLinterFunction*>(m_pOperand->GetIdentifier()->m_pIdentifier);
}

/***********************************************************************
 > 
***********************************************************************/
CASTOperand::CASTOperand(std::unique_ptr<AbstractSyntaxTree>&& ptr) : m_pAST(std::move(ptr)) {}
CASTOperand::~CASTOperand() = default;

CArrayOperand::CArrayOperand(ExpressionList&& ptr) : m_oExpressions(std::move(ptr)){}
CArrayOperand::~CArrayOperand() = default;


