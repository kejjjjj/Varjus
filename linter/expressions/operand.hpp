#pragma once

#include "definitions.hpp"

class CLinterOperand
{
public:

	CLinterOperand() = delete;
	explicit CLinterOperand(LinterIterator& pos, LinterIterator& end);
	~CLinterOperand();

	[[nodiscard]] Success ParseOperand();
	[[nodiscard]] std::string ToString() const noexcept;


private:
	std::vector<const CPunctuationToken*> m_oUnaryTokens;
	std::vector<const CToken*> m_oIdentifierTokens;
	std::vector<const CPunctuationToken*> m_oPostfixTokens;

	LinterIterator& m_iterPos;
	LinterIterator& m_iterEnd;
};
