#pragma once


#include "linter/expressions/definitions.hpp"

class CMemory;

class CImportLinter final : public CLinterSingle<CToken>
{
	NONCOPYABLE(CImportLinter);
public:
	explicit CImportLinter(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const owner);

	Success Parse();

private:

	Success ParseIdentifierRecursively();
	Success ParseFilePath();

	Success ParseFile();

	WeakScope m_pScope;
	CMemory* const m_pOwner;

	VectorOf<std::string> m_oNames;
	std::string m_oTargetFile;
};
