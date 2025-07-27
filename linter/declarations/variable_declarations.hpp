#pragma once

#include "varjus_api/internal/globalEnums.hpp"
#include "linter/expressions/definitions.hpp"
#include "varjus_api/internal/structure.hpp"

#include <variant>
#include <memory>

class CMemory;
struct CLinterVariable;
struct CConstEvalLinterVariable;

class CVariableDeclarationLinter final : public CLinterSingle<CToken>, protected IRuntimeBlock
{
	friend class CMemory;
	friend class CExportLinter;
	friend class CForStatementLinter;
	VARJUS_NONCOPYABLE(CVariableDeclarationLinter);

public:
	CVariableDeclarationLinter() = delete;
	CVariableDeclarationLinter(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const stack);
	~CVariableDeclarationLinter();

	[[nodiscard]] Varjus::Success Parse();

	[[nodiscard]] Varjus::Success ParseIdentifier();
	[[nodiscard]] Varjus::Success ParseInitializer();

	[[nodiscard]] __RuntimeBlock ToRuntimeObject() const override;
	[[nodiscard]] ASTNode&& MoveInitializer();

	[[nodiscard]] static bool IsDeclaration(const CToken* token) noexcept;
	[[nodiscard]] static bool IsIdentifier(const CToken* token) noexcept;

	[[nodiscard]] static CLinterVariable* DeclareVariable(const CToken* token, const WeakScope& scope, CMemory* const stack);

	[[nodiscard]] constexpr EInitializationTarget DeclarationType() const noexcept { return m_eType; }

private:

	WeakScope m_pScope;
	CMemory* const m_pOwner = 0;

	std::variant<CLinterVariable*, std::unique_ptr<CDestructuredData>> m_oDeclarationData;
	EInitializationTarget m_eType{};

	ASTNode m_pInitializerAST;
	bool m_bIsConst{};
};
