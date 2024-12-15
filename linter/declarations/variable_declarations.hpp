#pragma once

#include <string>
#include "globalEnums.hpp"
#include "linter/expressions/definitions.hpp"
#include "runtime/structure.hpp"

class CMemory;
struct CLinterVariable;

class CVariableDeclarationLinter final : public CLinterSingle<CToken>, protected IRuntimeBlock
{
	friend class CMemory;
	NONCOPYABLE(CVariableDeclarationLinter);

public:
	CVariableDeclarationLinter() = delete;
	CVariableDeclarationLinter(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const stack);
	~CVariableDeclarationLinter();

	[[nodiscard]] Success Parse();
	[[nodiscard]] RuntimeBlock ToRuntimeObject() const override;
	[[nodiscard]] std::unique_ptr<AbstractSyntaxTree>&& MoveInitializer();

	[[nodiscard]] constexpr auto& GetIdentifier() const noexcept { return m_sDeclaredVariable; }

	[[nodiscard]] static bool IsDeclaration(const CToken* token) noexcept;


private:
	[[nodiscard]] bool IsIdentifier(const CToken* token) const noexcept;

	WeakScope m_pScope;
	CMemory* const m_pOwner = 0;
	const CLinterVariable* m_sDeclaredVariable{ nullptr };

	std::unique_ptr<AbstractSyntaxTree> m_pInitializerAST;
};
