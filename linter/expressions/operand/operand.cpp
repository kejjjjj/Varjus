#include "operand.hpp"
#include "linter/expressions/unary.hpp"
#include "linter/expressions/postfix.hpp"
#include "linter/expressions/ast.hpp"

#include "linter/punctuation.hpp"
#include "linter/token.hpp"
#include "linter/functions/stack.hpp"

#include "globalEnums.hpp"

#include <cassert>


CLinterOperand::CLinterOperand(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const stack)
	:CLinterSingle(pos, end), m_pScope(scope), m_pOwner(stack) {

	assert(m_iterPos != m_iterEnd);

}
CLinterOperand::~CLinterOperand() = default;

Success CLinterOperand::ParseOperand(std::optional<PairMatcher>& eoe)
{
	// Parse unary
	CUnaryLinter unaryLinter(m_iterPos, m_iterEnd);
	if (!unaryLinter.ParseUnary())
		return failure;

	const auto token = (*m_iterPos);

	if (IS_IMMEDIATE(token->Type())) {
		m_pOperand = ParseImmediate();
	} else if (token->IsOperator(p_par_open)) {
		m_pOperand = ParseParentheses();
	} else if (token->IsOperator(p_bracket_open)) {
		m_pOperand = ParseArray();
	} else if (token->IsOperator(p_curlybracket_open)) {
		m_pOperand = ParseObject();
	} else {
		m_pOperand = ParseIdentifier();
	}

	CPostfixLinter postfix(m_iterPos, m_iterEnd, m_pScope, m_pOwner);
	if (!postfix.ParsePostfix())
		return failure;

	// Save results
	m_oUnaryTokens = unaryLinter.GetTokens();
	m_oPostfixes = postfix.Move();

	if (!IsEndOfBuffer() && (*m_iterPos)->IsOperator(p_question_mark)) {
		m_pOperand = ParseTernary(eoe);
		m_oPostfixes.clear();
		m_oUnaryTokens.clear();
	}

	return success;
}

std::unique_ptr<AbstractSyntaxTree> CLinterOperand::OperandToAST() const noexcept{
	return m_pOperand->ToAST();
}

std::unique_ptr<AbstractSyntaxTree> CLinterOperand::ToAST()
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

bool CLinterOperand::EndOfExpression(const std::optional<PairMatcher>& eoe, LinterIterator& pos) const noexcept
{

	if (!eoe)
		return (*pos)->IsOperator(p_semicolon);

	if (!(*pos)->IsOperator())
		return false;

	return eoe->IsClosing(dynamic_cast<const CPunctuationToken*>(*pos)->m_ePunctuation);
}