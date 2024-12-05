#include "unary.hpp"
#include "linter/token.hpp"
#include "linter/punctuation.hpp"

#include "linter/expressions/ast/ast_unary.hpp"

#include "globalEnums.hpp"

#include <cassert>
CUnaryLinter::CUnaryLinter(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const stack) 
	: CVectorLinter(pos, end), m_pScope(scope), m_pOwner(stack)
{
	assert(m_iterPos != m_iterEnd);
}
CUnaryLinter::~CUnaryLinter() = default;

#pragma pack(push)
#pragma warning( disable : 4061)
Success CUnaryLinter::ParseUnary()
{

	while(!IsEndOfBuffer() && (*m_iterPos)->IsOperator()) {
		const auto& asPunctuation = dynamic_cast<CPunctuationToken&>(**m_iterPos);

		if (!IsUnaryOperator(asPunctuation))
			break;

		switch (asPunctuation.m_ePunctuation) {
		case p_sub:
			m_oUnaryOperators.emplace_back(ParseNegation());
			break;
		case p_increment:
			m_oUnaryOperators.emplace_back(ParseIncrement());
			break;
		case p_decrement:
			m_oUnaryOperators.emplace_back(ParseDecrement());
			break;
		case p_exclamation:
			m_oUnaryOperators.emplace_back(ParseLogicalNot());
			break;
		default:
			assert(false);
		}

		if (m_oUnaryOperators.size())
			m_oUnaryOperators.back()->m_oCodePosition = asPunctuation.m_oSourcePosition;

		m_oTokens.emplace_back(&asPunctuation);
	}

	return success;
}
#pragma pack(pop)

bool CUnaryLinter::IsUnaryOperator(const CPunctuationToken& token) const noexcept
{
	return token.m_ePriority == op_unary || token.m_ePriority == op_additive;
}
VectorOf<std::unique_ptr<CUnaryBase>> CUnaryLinter::Move() noexcept {
	return std::move(m_oUnaryOperators);
}

std::unique_ptr<CUnaryBase> CUnaryLinter::ParseNegation() {
	assert(!IsEndOfBuffer() && (*m_iterPos)->IsOperator(p_sub));
	std::advance(m_iterPos, 1);
	return std::make_unique<CUnaryNegation>();
}
std::unique_ptr<AbstractSyntaxTree> CUnaryNegation::ToAST() {
	return std::make_unique<UnaryNegationAST>(m_oCodePosition);
}

std::unique_ptr<CUnaryBase> CUnaryLinter::ParseIncrement(){
	assert(!IsEndOfBuffer() && (*m_iterPos)->IsOperator(p_increment));
	std::advance(m_iterPos, 1);
	return std::make_unique<CUnaryIncrement>();
}
std::unique_ptr<AbstractSyntaxTree> CUnaryIncrement::ToAST() {
	return std::make_unique<UnaryIncrementAST>(m_oCodePosition);
}

std::unique_ptr<CUnaryBase> CUnaryLinter::ParseDecrement(){
	assert(!IsEndOfBuffer() && (*m_iterPos)->IsOperator(p_decrement));
	std::advance(m_iterPos, 1);
	return std::make_unique<CUnaryDecrement>();
}
std::unique_ptr<AbstractSyntaxTree> CUnaryDecrement::ToAST() {
	return std::make_unique<UnaryDecrementAST>(m_oCodePosition);
}

std::unique_ptr<CUnaryBase> CUnaryLinter::ParseLogicalNot()
{
	assert(!IsEndOfBuffer() && (*m_iterPos)->IsOperator(p_exclamation));
	std::advance(m_iterPos, 1);
	return std::make_unique<CUnaryLogicalNot>();
}
std::unique_ptr<AbstractSyntaxTree> CUnaryLogicalNot::ToAST() {
	return std::make_unique<UnaryLogicalNotAST>(m_oCodePosition);
}