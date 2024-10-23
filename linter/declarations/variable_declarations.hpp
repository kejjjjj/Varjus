#pragma once

#include <string>
#include "globalEnums.hpp"

#include "linter/expressions/definitions.hpp"

class CMemory;

class CVariableDeclaration final : public CLinterSingle<CToken>
{
	friend class CMemory;

public:
	CVariableDeclaration() = delete;
	CVariableDeclaration(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const stack);
	CVariableDeclaration operator=(const CVariableDeclaration&) = delete;

	[[nodiscard]] Success ParseDeclaration();

	[[nodiscard]] bool IsGlobalVariable() const noexcept;

private:
	[[nodiscard]] bool IsDeclaration(const CToken* token) const noexcept;
	[[nodiscard]] bool IsIdentifier(const CToken* token) const noexcept;

	WeakScope m_pScope;
	CMemory* const m_pOwner = 0;
};
