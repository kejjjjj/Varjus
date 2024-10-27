
#pragma once

#include "expressions/definitions.hpp"

class CMemory;
class IRuntimeStructure;
class CFileRuntimeData;

using RuntimeBlock = std::unique_ptr<IRuntimeStructure>;

class CFileLinter final : public CLinter<CToken>
{
	NONCOPYABLE(CFileLinter)
public:
	CFileLinter(LinterIterator& start, LinterIterator& end);

	[[nodiscard]] static Success LintToken(LinterIterator& start, LinterIterator& end, const WeakScope& scope, CMemory* const stack);
	[[nodiscard]] static Success LintOperator(LinterIterator& start, LinterIterator& end, const WeakScope& scope, CMemory* const stack);
	[[nodiscard]] static Success LintExpression(LinterIterator& start, LinterIterator& end, const WeakScope& scope, CMemory* const stack);
	[[nodiscard]] static Success LintDeclaration(LinterIterator& start, LinterIterator& end, const WeakScope& scope, CMemory* const stack);
	[[nodiscard]] static Success LintFunction(LinterIterator& start, LinterIterator& end, const WeakScope& scope, CMemory* const stack);

	[[nodiscard]] Success ParseFile();

private:
	std::unique_ptr<CFileRuntimeData> m_pFile;
};


