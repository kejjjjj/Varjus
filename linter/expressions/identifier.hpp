#pragma once

#include "definitions.hpp"

class CMemoryData;
struct CLinterVariable;

class CIdentifierLinter : public CLinterSingle<CToken>
{
	friend class CLinterOperand;
public:
	CIdentifierLinter() = delete;
	explicit CIdentifierLinter(LinterIterator& pos, LinterIterator& end, CMemoryData* const stack);
	~CIdentifierLinter();

	[[nodiscard]] Success ParseIdentifier();

	[[nodiscard]] auto GetResult() const { return m_pToken; }

private:
	[[nodiscard]] bool CheckIdentifier(const CToken* token) const noexcept;

	CMemoryData* const m_pOwner;
	CLinterVariable* m_pVariable{};

};