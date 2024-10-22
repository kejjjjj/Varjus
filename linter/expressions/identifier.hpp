#pragma once

#include "definitions.hpp"

class CMemory;
struct CLinterVariable;

class CIdentifierLinter final : public CLinterSingle<CToken>
{
	friend class CLinterOperand;
public:
	CIdentifierLinter() = delete;
	explicit CIdentifierLinter(LinterIterator& pos, LinterIterator& end, CMemory* const stack);
	~CIdentifierLinter();

	[[nodiscard]] Success ParseIdentifier();
	[[nodiscard]] auto GetResult() const { return m_pToken; }

	[[nodiscard]] std::int64_t ToInt() const noexcept;

private:
	[[nodiscard]] bool CheckIdentifier(const CToken* token) const noexcept;

	CMemory* const m_pOwner;
	CLinterVariable* m_pVariable{};
};