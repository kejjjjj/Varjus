#pragma once

#include "linter/statements/statement.hpp"

class IRuntimeStructure;
class AbstractSyntaxTree;
class CScope;
struct CFunctionBlock;

class CTryCatchStatementLinter final : public CStatementLinter, protected IRuntimeBlock
{
	VARJUS_NONCOPYABLE(CTryCatchStatementLinter);

public:
	explicit CTryCatchStatementLinter(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const owner);
	~CTryCatchStatementLinter();
	[[nodiscard]] Varjus::Success Parse();

	[[nodiscard]] __RuntimeBlock ToRuntimeObject() const override;
private:

	Varjus::Success ParseCatchStatement();

	CCrossModuleReference m_uCatchVariable;
	VectorOf<__RuntimeBlock> m_oTryInstructions;
	VectorOf<__RuntimeBlock> m_oCatchInstructions;
};