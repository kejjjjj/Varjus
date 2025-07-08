#pragma once

#include "linter/statements/statement.hpp"

class IRuntimeStructure;
class AbstractSyntaxTree;
class CScope;
struct CFunctionBlock;

class CElseStatementLinter final : public CStatementLinter, protected IRuntimeBlock
{
	VARJUS_NONCOPYABLE(CElseStatementLinter);

public:
	explicit CElseStatementLinter(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const owner);
	~CElseStatementLinter();

	[[nodiscard]] Varjus::Success Parse();
	[[nodiscard]] bool IsInConditionalContext() const;

	[[nodiscard]] RuntimeBlock ToRuntimeObject() const override;

private:
	CRuntimeConditionalStatement* m_pPreviousBlock{ nullptr };
};