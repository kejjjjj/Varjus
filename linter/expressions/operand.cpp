#include "operand.hpp"
#include "unary.hpp"
#include "identifier.hpp"

#include "linter/token.hpp"
#include "linter/error.hpp"
#include "linter/declarations/stack.hpp"
#include "globalEnums.hpp"

#include <cassert>

CLinterOperand::CLinterOperand(LinterIterator& pos, LinterIterator& end, CMemoryData* const stack) 
	: m_iterPos(pos), m_iterEnd(end), m_pOwner(stack) {

	assert(m_iterPos != m_iterEnd);

}
CLinterOperand::~CLinterOperand() = default;

Success CLinterOperand::ParseOperand()
{
	// Parse unary
	CUnaryLinter unaryLinter(m_iterPos, m_iterEnd);
	unaryLinter.ParseUnary();

	// Parse identifier
	CIdentifierLinter identifierLinter(m_iterPos, m_iterEnd, m_pOwner);
	if (!identifierLinter.ParseIdentifier()) {
		return failure;
	}



	//TODO: Parse postfix


	// Save results
	m_oUnaryTokens = unaryLinter.GetResult();
	m_oIdentifierToken = identifierLinter.GetResult();

	return success;
}
std::string CLinterOperand::ToString() const noexcept
{
	std::string result;
	for (const auto& unary : m_oUnaryTokens) {
		result += unary->Source();
	}
	result += m_oIdentifierToken->Source();
	return result;
}
