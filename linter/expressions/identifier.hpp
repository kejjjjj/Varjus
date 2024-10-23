#pragma once

#include "definitions.hpp"

class CMemory;
class CScope;
struct CLinterVariable;

class CIdentifierLinter final : public CLinterSingle<CToken>
{
	friend class CLinterOperand;
public:
	CIdentifierLinter() = delete;
	explicit CIdentifierLinter(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const stack);
	~CIdentifierLinter();

	[[nodiscard]] Success ParseIdentifier();
	[[nodiscard]] auto GetResult() const { return m_pToken; }

	[[nodiscard]] std::int64_t ToInt() const noexcept;

private:
	[[nodiscard]] bool CheckIdentifier(const CToken* token) const noexcept;

	WeakScope m_pScope;
	CMemory* const m_pOwner;
	CLinterVariable* m_pVariable{};
};