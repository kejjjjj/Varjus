#pragma once


#include "varjus_api/internal/globalEnums.hpp"
#include "linter/expressions/definitions.hpp"
#include "varjus_api/internal/structure.hpp"

class CMemory;
struct CLinterVariable;
struct CConstEvalLinterVariable;

class CVariableDeclarationLinter final : public CLinterSingle<CToken>, protected IRuntimeBlock
{
	friend class CMemory;
	VARJUS_NONCOPYABLE(CVariableDeclarationLinter);

public:
	CVariableDeclarationLinter() = delete;
	CVariableDeclarationLinter(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const stack);
	~CVariableDeclarationLinter();

	[[nodiscard]] Varjus::Success Parse();

	[[nodiscard]] Varjus::Success ParseIdentifier();
	[[nodiscard]] Varjus::Success ParseInitializer();

	[[nodiscard]] RuntimeBlock ToRuntimeObject() const override;
	[[nodiscard]] ASTNode&& MoveInitializer();

	[[nodiscard]] constexpr auto& GetIdentifier() const noexcept { return m_sDeclaredVariable; }

	[[nodiscard]] static bool IsDeclaration(const CToken* token) noexcept;


private:
	[[nodiscard]] bool IsIdentifier(const CToken* token) const noexcept;

	WeakScope m_pScope;
	CMemory* const m_pOwner = 0;
	CLinterVariable* m_sDeclaredVariable{ nullptr };
	ASTNode m_pInitializerAST;
	bool m_bIsConst{};
};
