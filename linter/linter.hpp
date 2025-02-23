
#pragma once

#include "expressions/definitions.hpp"

class CMemory;
class IRuntimeStructure;
class CModule;
class CHoister;

struct CFileContext;

using RuntimeBlock = std::unique_ptr<IRuntimeStructure>;

WARNING_PUSH()
WARNING_DISABLE(5027)
WARNING_DISABLE(4623)
struct CLinterContext
{
	LinterIterator& m_iterPos;
	LinterIterator& m_iterEnd;
	const WeakScope& scope;
	CMemory* const memory;
	bool m_bAddInstructions{ true };
};
WARNING_POP()

class CBufferLinter final : public CLinter<CToken>
{
	NONCOPYABLE(CBufferLinter);
public:
	CBufferLinter(LinterIterator& start, LinterIterator& end, const std::string& filePath="");
	~CBufferLinter();
	[[nodiscard]] static Success LintToken(const CLinterContext& ctx);
	[[nodiscard]] static Success LintOperator(const CLinterContext& ctx);
	[[nodiscard]] static Success LintScope(const CLinterContext& ctx);
	[[nodiscard]] static Success LintFunctionAmbiguity(const CLinterContext& ctx);

	[[nodiscard]] Success Parse();

	constexpr auto& GetModule() const noexcept { return m_pModule; }

private:
	[[nodiscard]] Success HoistFile();
	[[nodiscard]] Success LintFile();


	CModule* m_pModule{ nullptr };
	LinterIterator m_oInitialPosition;
	std::unique_ptr<CHoister> m_pHoister;
	std::string m_sFilePath;
};


