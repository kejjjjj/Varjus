#pragma once

#include <string>
#include "globalEnums.hpp"

#include "linter/expressions/definitions.hpp"

class CMemoryData;

class CVariableDeclaration : public CLinterSingle<CToken>
{
	friend class CMemoryData;

public:
	CVariableDeclaration() = delete;
	CVariableDeclaration(LinterIterator& pos, LinterIterator& end, CMemoryData* const stack);
	CVariableDeclaration operator=(const CVariableDeclaration&) = delete;

	[[nodiscard]] Success ParseDeclaration();

	[[nodiscard]] bool IsGlobalVariable() const noexcept;

private:
	[[nodiscard]] bool IsDeclaration(const CToken* token) const noexcept;
	[[nodiscard]] bool IsIdentifier(const CToken* token) const noexcept;

	CMemoryData* const m_pOwner = 0;
};
