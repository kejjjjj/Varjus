#pragma once


#include "linter/expressions/definitions.hpp"
#include "varjus_api/internal/structure.hpp"

class CMemory;
class CVariableDeclarationLinter;

class CExportLinter final : public CLinterSingle<CToken>, protected IRuntimeBlock
{
	VARJUS_NONCOPYABLE(CExportLinter);
public:
	explicit CExportLinter(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const owner);
	~CExportLinter();

	[[nodiscard]] Varjus::Success Parse();
	[[nodiscard]] __RuntimeBlock ToRuntimeObject() const override;

private:
	[[nodiscard]] Varjus::Success ParseVariableDeclaration();
	[[nodiscard]] Varjus::Success ParseFunctionDeclaration();


	WeakScope m_pScope;
	CMemory* const m_pOwner;

	std::unique_ptr<CVariableDeclarationLinter> m_pVariableLinter;

};

enum EExportedSymbol
{
	es_variable,
	es_function
};

class CExportedSymbol
{
	VARJUS_NONCOPYABLE(CExportedSymbol);
public:
	CExportedSymbol(std::size_t i) : m_uIndex(i) {}
	virtual ~CExportedSymbol() = default;

	[[nodiscard]] virtual constexpr EExportedSymbol Type() const noexcept = 0;
	[[nodiscard]] constexpr auto& GetIndex() const noexcept { return m_uIndex; }
protected:
	std::size_t m_uIndex{};
};
class CExportedVariable : public CExportedSymbol
{
	VARJUS_NONCOPYABLE(CExportedVariable);
public:
	CExportedVariable(std::size_t i) : CExportedSymbol(i){}
	[[nodiscard]] constexpr EExportedSymbol Type() const noexcept override { return es_variable; }
};
class CExportedFunction : public CExportedSymbol
{
	VARJUS_NONCOPYABLE(CExportedFunction);
public:
	CExportedFunction(std::size_t i) : CExportedSymbol(i) {}
	[[nodiscard]] constexpr EExportedSymbol Type() const noexcept override { return es_function; }
};