
#pragma once

#include "expressions/definitions.hpp"

class CMemory;
class CCodeStructure;

using RuntimeBlock = std::unique_ptr<CCodeStructure>;

class CFileLinter final : public CLinter<CToken>
{
	NONCOPYABLE(CFileLinter)
public:
	CFileLinter(LinterIterator& start, LinterIterator& end);

	[[nodiscard]] static Success LintToken(LinterIterator& start, LinterIterator& end, const WeakScope& scope, CMemory* const stack);

	static Success LintOperator(LinterIterator& start, LinterIterator& end, const WeakScope& scope, CMemory* const stack);

	static Success LintExpression(LinterIterator& start, LinterIterator& end, const WeakScope& scope, CMemory* const stack);
	static Success LintDeclaration(LinterIterator& start, LinterIterator& end, const WeakScope& scope, CMemory* const stack);
	static Success LintFunction(LinterIterator& start, LinterIterator& end, const WeakScope& scope, CMemory* const stack);

	[[nodiscard]] Success ParseFile();
private:


};


