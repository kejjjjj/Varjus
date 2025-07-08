#pragma once

#include "linter/statements/statement.hpp"

class IRuntimeStructure;
class AbstractSyntaxTree;
class CScope;
struct CFunctionBlock;


class CWhileStatementLinter final : public CStatementLinter, protected IRuntimeBlock
{
	VARJUS_NONCOPYABLE(CWhileStatementLinter);

public:
	explicit CWhileStatementLinter(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const owner);
	~CWhileStatementLinter();
	[[nodiscard]] Varjus::Success Parse();

	[[nodiscard]] RuntimeBlock ToRuntimeObject() const override;

private:

	ASTNode m_pCondition;
};