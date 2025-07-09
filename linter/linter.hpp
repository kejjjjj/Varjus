
#pragma once

#include "expressions/definitions.hpp"

class CMemory;
class IRuntimeStructure;
class CModule;
class CHoister;

using __RuntimeBlock = std::unique_ptr<IRuntimeStructure>;

VARJUS_WARNING_PUSH()
VARJUS_WARNING_DISABLE(5027)
VARJUS_WARNING_DISABLE(4623)
struct CLinterContext
{
	LinterIterator& m_iterPos;
	LinterIterator& m_iterEnd;
	const WeakScope& scope;
	CMemory* const memory;
	CModule* const m_pModule;
	bool m_bAddInstructions{ true };
};
VARJUS_WARNING_POP()

class CBufferLinter final : public CLinter<CToken>
{
	VARJUS_NONCOPYABLE(CBufferLinter);
public:
	CBufferLinter(Varjus::CProgramInformation* const program, LinterIterator& start, LinterIterator& end, const VarjusString& filePath=VSL(""));
	~CBufferLinter();
	[[nodiscard]] static Varjus::Success LintToken(const CLinterContext& ctx);
	[[nodiscard]] static Varjus::Success LintOperator(const CLinterContext& ctx);
	[[nodiscard]] static Varjus::Success LintScope(const CLinterContext& ctx);
	[[nodiscard]] static Varjus::Success LintFunctionAmbiguity(const CLinterContext& ctx);

	[[nodiscard]] Varjus::Success Parse();

	constexpr auto& GetModule() const noexcept { return m_pModule; }

private:
	[[nodiscard]] Varjus::Success HoistFile();
	[[nodiscard]] Varjus::Success LintFile();


	CModule* m_pModule{ nullptr };
	LinterIterator m_oInitialPosition;
	std::unique_ptr<CHoister> m_pHoister;
	VarjusString m_sFilePath;
	Varjus::CProgramInformation* const m_pProgram{};
};


