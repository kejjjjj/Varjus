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
		case p_increment:
			m_oUnaryOperators.emplace_back(ParseIncrement());
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

std::unique_ptr<CUnaryIncrement> CUnaryLinter::ParseIncrement(){
	assert(!IsEndOfBuffer() && (*m_iterPos)->IsOperator(p_increment));
	std::advance(m_iterPos, 1);
	return std::make_unique<CUnaryIncrement>();
}
std::unique_ptr<AbstractSyntaxTree> CUnaryIncrement::ToAST() {
	return std::make_unique<UnaryIncrementAST>(m_oCodePosition);
}
