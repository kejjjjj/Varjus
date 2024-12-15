#pragma once


#include "linter/expressions/definitions.hpp"

class CMemory;
class CModule;

class CImportLinter final : public CLinterSingle<CToken>
{
	NONCOPYABLE(CImportLinter);
public:
	explicit CImportLinter(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const owner);

	[[nodiscard]] Success Parse();

private:

	[[nodiscard]] Success ParseIdentifierRecursively();
	[[nodiscard]] Success ParseFilePath();

	[[nodiscard]] Success ParseFile();
	[[nodiscard]] CModule* GetFileModule() const;

	WeakScope m_pScope;
	CMemory* const m_pOwner;

	VectorOf<std::string> m_oNames;
	std::string m_oTargetFile;
};
