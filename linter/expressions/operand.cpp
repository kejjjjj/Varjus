#include "operand.hpp"
#include "unary.hpp"
#include "identifier.hpp"

#include "linter/token.hpp"
#include "linter/error.hpp"
#include "globalEnums.hpp"

#include <cassert>

CLinterOperand::CLinterOperand(LinterIterator& pos, LinterIterator& end) : m_iterPos(pos), m_iterEnd(end) {

	assert(m_iterPos != m_iterEnd);

}
CLinterOperand::~CLinterOperand() = default;

Success CLinterOperand::ParseOperand()
{
	// Parse unary
	CUnaryLinter unaryLinter(m_iterPos, m_iterEnd);
	unaryLinter.ParseUnary();

	// Parse identifier
	CIdentifierLinter identifierLinter(m_iterPos, m_iterEnd);
	if (!identifierLinter.ParseIdentifier()) {
		return failure;
	}

	//TODO: Parse postfix


	// Save results
	m_oUnaryTokens = unaryLinter.GetResult();
	m_oIdentifierTokens = identifierLinter.GetResult();

	return success;
}
std::string CLinterOperand::ToString() const noexcept
{
	std::string result;
	for (const auto& unary : m_oUnaryTokens) {
		result += unary->Source();
	}
	for (const auto& identifier : m_oIdentifierTokens) {
		result += identifier->Source();
	}
	return result;
}
