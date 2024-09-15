#pragma once

#include "definitions.hpp"

class CMemoryData;

class CIdentifierLinter : public CLinterSingle<CToken>
{
public:
	CIdentifierLinter() = delete;
	explicit CIdentifierLinter(LinterIterator& pos, LinterIterator& end, CMemoryData* const stack);
	~CIdentifierLinter();

	[[nodiscard]] Success ParseIdentifier();

	[[nodiscard]] auto GetResult() const { return m_pToken; }

private:
	[[nodiscard]] bool CheckIdentifier(const CToken* token) const noexcept;

	CMemoryData* const m_pOwner;
};