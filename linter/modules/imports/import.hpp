#pragma once


#include "linter/expressions/definitions.hpp"

class CMemory;
class CModule;
class CExportedSymbol;
class CScope;

class CImportLinter final : public CLinterSingle<CToken>
{
	VARJUS_NONCOPYABLE(CImportLinter);
public:
	explicit CImportLinter(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const owner);

	[[nodiscard]] Varjus::Success Parse();

private:

	[[nodiscard]] Varjus::Success ParseIdentifierRecursively();
	[[nodiscard]] Varjus::Success ParseFilePath();

	[[nodiscard]] Varjus::Success ParseFile();
	[[nodiscard]] CModule* GetFileModule() const;

	[[nodiscard]] Varjus::Success DeclareVariable(const VarjusString& symbolName, CExportedSymbol* const s, std::size_t moduleIndex);
	[[nodiscard]] Varjus::Success DeclareFunction(const VarjusString& symbolName, CExportedSymbol* const s, std::size_t moduleIndex);

	WeakScope m_pScope;
	CMemory* const m_pOwner;

	VectorOf<VarjusString> m_oNames;
	VarjusString m_oTargetFile;
};
