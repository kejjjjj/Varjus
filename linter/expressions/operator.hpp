#pragma once

#include "identifier.hpp"

class CLinterOperatorParser : public CLinter<CPunctuationToken>
{
public:
	CLinterOperatorParser() = delete;
	explicit CLinterOperatorParser(LinterIterator& pos, LinterIterator& end);
	~CLinterOperatorParser();

	[[maybe_unused]] Success ParseOperator();
	[[nodiscard]] auto& GetResult() const { return m_oTokens; }

private:
	[[nodiscard]] bool CheckOperator() const;
	[[nodiscard]] bool IsOperator(const CPunctuationToken& token) const noexcept;
};
