
#pragma once

#include "expressions/definitions.hpp"

class CMemory;
class IRuntimeStructure;
class CFileRuntimeData;
struct CProgramContext;

using RuntimeBlock = std::unique_ptr<IRuntimeStructure>;

class CFileLinter final : public CLinter<CToken>
{
	NONCOPYABLE(CFileLinter)
public:
	CFileLinter(LinterIterator& start, LinterIterator& end, CProgramContext* const context);
	~CFileLinter();
	[[nodiscard]] static Success LintToken(LinterIterator& start, LinterIterator& end, const WeakScope& scope, CMemory* const stack);
	[[nodiscard]] static Success LintOperator(LinterIterator& start, LinterIterator& end, const WeakScope& scope, CMemory* const stack);

	[[nodiscard]] Success ParseFile();

	CFileRuntimeData* GetRuntimeInformation() const noexcept;

private:
	std::unique_ptr<CFileRuntimeData> m_pFile;
	CProgramContext* const m_pContext;
};


