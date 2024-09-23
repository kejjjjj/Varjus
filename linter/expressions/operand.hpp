#pragma once

#include "definitions.hpp"

class CMemoryData;
class CIdentifierLinter;

struct CLinterVariable;

class CLinterOperand
{
	NONCOPYABLE(CLinterOperand);
public:

	CLinterOperand() = delete;
	explicit CLinterOperand(LinterIterator& pos, LinterIterator& end, CMemoryData* const stack);
	~CLinterOperand();

	[[nodiscard]] Success ParseOperand();
	
	[[nodiscard]] std::string ToString() const noexcept;

	[[nodiscard]] bool IsVariable() const noexcept;
	[[nodiscard]] bool IsImmediate() const noexcept;

	[[nodiscard]] const CLinterVariable* GetVariable() const noexcept;

	[[nodiscard]] const CIdentifierLinter* GetOperand() const noexcept { return m_oIdentifierToken.get(); }

private:
	std::vector<const CPunctuationToken*> m_oUnaryTokens;
	std::unique_ptr<CIdentifierLinter> m_oIdentifierToken;
	std::vector<const CPunctuationToken*> m_oPostfixTokens;

	LinterIterator& m_iterPos;
	LinterIterator& m_iterEnd;

	CMemoryData* const m_pOwner;

};
