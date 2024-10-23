
#pragma once

#include "expressions/definitions.hpp"

class CMemory;

class CFileLinter final : public CLinter<CToken>
{
	NONCOPYABLE(CFileLinter)
public:
	CFileLinter(LinterIterator& start, LinterIterator& end);

	[[nodiscard]] static Success LintToken(LinterIterator& start, LinterIterator& end, const WeakScope& scope, CMemory* const stack);

	static void LintOperator(LinterIterator& start, LinterIterator& end, const WeakScope& scope, CMemory* const stack);

	static void LintExpression(LinterIterator& start, LinterIterator& end, const WeakScope& scope, CMemory* const stack);
	static void LintDeclaration(LinterIterator& start, LinterIterator& end, const WeakScope& scope, CMemory* const stack);
	static void LintFunction(LinterIterator& start, LinterIterator& end, const WeakScope& scope, CMemory* const stack);

	[[nodiscard]] Success ParseFile();
private:
};


