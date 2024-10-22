
#pragma once

#include "expressions/definitions.hpp"

class CScope;
class CMemory;

class CFileLinter final : public CLinter<CToken>
{
	NONCOPYABLE(CFileLinter)
public:
	CFileLinter(LinterIterator& start, LinterIterator& end);

	[[nodiscard]] static Success LintToken(LinterIterator& start, LinterIterator& end, CMemory* const stack);

	static void LintExpression(LinterIterator& start, LinterIterator& end, CMemory* const stack);
	static void LintDeclaration(LinterIterator& start, LinterIterator& end, CMemory* const stack);
	static void LintFunction(LinterIterator& start, LinterIterator& end, CMemory* const stack);

	[[nodiscard]] Success ParseFile();

	static std::weak_ptr<CScope> GetCurrentScope();
private:
	static std::weak_ptr<CScope> m_pCurrentScope;
};


