#pragma once

#include <string>
#include "globalEnums.hpp"
#include "linter/expressions/definitions.hpp"
#include "runtime/structure.hpp"

class CMemory;

class CVariableDeclaration final : public CLinterSingle<CToken>, protected IRuntimeBlock
{
	friend class CMemory;
	NONCOPYABLE(CVariableDeclaration);

public:
	CVariableDeclaration() = delete;
	CVariableDeclaration(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const stack);
	~CVariableDeclaration();

	[[nodiscard]] Success ParseDeclaration();

	[[nodiscard]] bool IsGlobalVariable() const noexcept;

	[[nodiscard]] RuntimeBlock ToRuntimeObject() const override;


private:
	[[nodiscard]] bool IsDeclaration(const CToken* token) const noexcept;
	[[nodiscard]] bool IsIdentifier(const CToken* token) const noexcept;

	WeakScope m_pScope;
	CMemory* const m_pOwner = 0;

	std::unique_ptr<AbstractSyntaxTree> m_pAST;
};
