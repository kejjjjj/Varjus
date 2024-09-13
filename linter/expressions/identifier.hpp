#pragma once

#include "definitions.hpp"


class CIdentifierLinter : public CLinter<CToken>
{
public:
	CIdentifierLinter() = delete;
	explicit CIdentifierLinter(LinterIterator& pos, LinterIterator& end);
	~CIdentifierLinter();

	[[nodiscard]] Success ParseIdentifier();

	[[nodiscard]] auto& GetResult() const { return m_oTokens; }

private:
	[[nodiscard]] bool CheckIdentifier(const CToken* token) const noexcept;
};