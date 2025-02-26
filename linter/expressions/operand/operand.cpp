#include "operand.hpp"
#include "linter/expressions/unary.hpp"
#include "linter/expressions/postfix.hpp"
#include "linter/expressions/ast.hpp"

#include "linter/punctuation.hpp"
#include "linter/token.hpp"
#include "linter/functions/stack.hpp"
#include "linter/error.hpp"
#include "linter/modules/module.hpp"

#include "varjus_api/internal/globalEnums.hpp"

#include <cassert>


CLinterOperand::CLinterOperand(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const stack)
	:CLinterSingle(pos, end), m_pScope(scope), m_pOwner(stack) {

	assert(m_iterPos != m_iterEnd);

}
CLinterOperand::~CLinterOperand() = default;

Success CLinterOperand::ParseOperand(std::optional<PairMatcher>& eoe)
{
	// Parse unary
	CUnaryLinter unaryLinter(m_iterPos, m_iterEnd, m_pScope, m_pOwner);
	if (!unaryLinter.ParseUnary())
		return failure;

	if (m_iterPos == m_iterEnd) {
		m_pOwner->GetModule()->PushError("unexpected end of buffer");
		return failure;
	}

	const auto token = (*m_iterPos);

	if (IS_IMMEDIATE(token->Type())) {
		m_pOperand = ParseImmediate();
	} else if (token->IsOperator(p_par_open)) {
		m_pOperand = ParseParentheses(eoe);
	} else if (token->IsOperator(p_bracket_open)) {
		m_pOperand = ParseArray();
	} else if (token->IsOperator(p_curlybracket_open)) {
		m_pOperand = ParseObject();
	} else if (token->Type() == tt_fn) {
		m_pOperand = ParseLambda();
	} else if (token->Type() == tt_fmt_string) {
		m_pOperand = ParseFormatString();
	} else {
		m_pOperand = ParseIdentifier();
	}

	if (m_iterPos == m_iterEnd) {
		m_pOwner->GetModule()->PushError("unexpected end of buffer");
		return failure;
	}

	CPostfixLinter postfix(m_iterPos, m_iterEnd, m_pScope, m_pOwner);
	if (!postfix.ParsePostfix())
		return failure;

	// Save results
	m_oUnaryOperators = unaryLinter.Move();
	m_oPostfixes = postfix.Move();

	if (!IsEndOfBuffer() && (*m_iterPos)->IsOperator(p_question_mark)) {
		m_pOperand = ParseTernary(eoe);
		m_oPostfixes.clear();
		m_oUnaryOperators.clear();
	}

	return success;
}

ASTNode CLinterOperand::OperandToAST() const noexcept{
	return m_pOperand->ToAST();
}

static AbstractSyntaxTree* SeekASTLeftBranch(AbstractSyntaxTree* src) {
	auto end = src;
	while (end->left)
		end = end->left.get();

	return end;
}

ASTNode CLinterOperand::ToAST()
{
	ASTNode m_pEntry{ nullptr };

	if (auto unaries = UnariesToAST()) {
		m_pEntry = std::move(unaries);
	}

	if (auto pfs = PostfixesToAST()) {
		if (!m_pEntry)
			m_pEntry = std::move(pfs);
		else
			SeekASTLeftBranch(m_pEntry.get())->left = std::move(pfs);

	}

	if(!m_pEntry)
		return OperandToAST();

	SeekASTLeftBranch(m_pEntry.get())->left = OperandToAST();
	return m_pEntry;
}

ASTNode CLinterOperand::PostfixesToAST() const noexcept
{
	if (!m_oPostfixes.size())
		return nullptr;

	ASTNode root;
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
ASTNode CLinterOperand::UnariesToAST() const noexcept
{
	if (!m_oUnaryOperators.size())
		return nullptr;

	std::unique_ptr<AbstractSyntaxTree> root;
	AbstractSyntaxTree* position{ nullptr };

	for (auto& unary : m_oUnaryOperators) {

		if (!root) {
			root = unary->ToAST();
			position = root.get();
			continue;
		}

		assert(position);

		position->left = unary->ToAST();
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