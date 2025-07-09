#include "unary.hpp"
#include "linter/token.hpp"
#include "linter/punctuation.hpp"

#include "linter/expressions/ast/ast_unary.hpp"
#include "linter/error.hpp"

#include "varjus_api/internal/globalEnums.hpp"

#include <cassert>

using namespace Varjus;

CUnaryLinter::CUnaryLinter(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const stack) 
	: CVectorLinter(pos, end), m_pScope(scope), m_pOwner(stack)
{
	assert(m_iterPos != m_iterEnd);
}
CUnaryLinter::~CUnaryLinter() = default;

#pragma pack(push)
WARNING_DISABLE(4061)
Varjus::Success CUnaryLinter::ParseUnary()
{

	while(!IsEndOfBuffer() && ((*m_iterPos)->IsOperator() 
		|| (*m_iterPos)->Type() == tt_typeof
		|| (*m_iterPos)->Type() == tt_tostring)) {

		auto pos = *m_iterPos;

		if ((*m_iterPos)->Type() == tt_typeof) {
			m_oUnaryOperators.emplace_back(ParseTypeOf());
		} else if ((*m_iterPos)->Type() == tt_tostring) {
			m_oUnaryOperators.emplace_back(ParseToString());
		} else {
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
			case p_tilde:
				m_oUnaryOperators.emplace_back(ParseBitwiseNot());
				break;
			default:
				assert(false);
			}
		}

		if (m_oUnaryOperators.size())
			m_oUnaryOperators.back()->m_oCodePosition = pos->m_oSourcePosition;

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

std::unique_ptr<CUnaryBase> CUnaryLinter::ParseBitwiseNot()
{
	assert(!IsEndOfBuffer() && (*m_iterPos)->IsOperator(p_tilde));
	std::advance(m_iterPos, 1);
	return std::make_unique<CUnaryBitwiseNot>();
}
std::unique_ptr<AbstractSyntaxTree> CUnaryBitwiseNot::ToAST() {
	return std::make_unique<UnaryBitwiseNotAST>(m_oCodePosition);
}

std::unique_ptr<CUnaryBase> CUnaryLinter::ParseTypeOf()
{
	assert(!IsEndOfBuffer() && (*m_iterPos)->Type() == tt_typeof);
	std::advance(m_iterPos, 1);
	return std::make_unique<CUnaryTypeOf>();
}
std::unique_ptr<CUnaryBase> CUnaryLinter::ParseToString()
{
	assert(!IsEndOfBuffer() && (*m_iterPos)->Type() == tt_tostring);
	std::advance(m_iterPos, 1);
	return std::make_unique<CUnaryToString>();
}
std::unique_ptr<AbstractSyntaxTree> CUnaryTypeOf::ToAST() {
	return std::make_unique<UnaryTypeOfAST>(m_oCodePosition);
}
std::unique_ptr<AbstractSyntaxTree> CUnaryToString::ToAST() {
	return std::make_unique<UnaryToStringAST>(m_oCodePosition);
}