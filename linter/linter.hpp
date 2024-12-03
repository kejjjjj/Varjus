
#pragma once

#include "expressions/definitions.hpp"

class CMemory;
class IRuntimeStructure;
class CFileRuntimeData;
class CHoister;

struct CProgramContext;

using RuntimeBlock = std::unique_ptr<IRuntimeStructure>;

#pragma warning(push)
#pragma warning(disable : 5027)
#pragma warning(disable : 4623)
struct CLinterContext
{
	LinterIterator& m_iterPos;
	LinterIterator& m_iterEnd;
	const WeakScope& scope;
	CMemory* const memory;
	bool m_bAddInstructions{ true };
};
#pragma warning(pop)

class CFileLinter final : public CLinter<CToken>
{
	NONCOPYABLE(CFileLinter)
public:
	CFileLinter(LinterIterator& start, LinterIterator& end, CProgramContext* const context);
	~CFileLinter();
	[[nodiscard]] static Success LintToken(const CLinterContext& ctx);
	[[nodiscard]] static Success LintOperator(const CLinterContext& ctx);
	[[nodiscard]] static Success LintScope(const CLinterContext& ctx);
	[[nodiscard]] static Success LintFunctionAmbiguity(const CLinterContext& ctx);

	[[nodiscard]] Success ParseFile();

	CFileRuntimeData* GetRuntimeInformation() const noexcept;

private:
	[[nodiscard]] Success HoistFile();
	[[nodiscard]] Success LintFile();


	std::unique_ptr<CFileRuntimeData> m_pFile;
	CProgramContext* const m_pContext;
	LinterIterator m_oInitialPosition;
	std::unique_ptr<CHoister> m_pHoister;

};


