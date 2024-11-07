#pragma once

#include "linter/statements/statement.hpp"

class IRuntimeStructure;
class AbstractSyntaxTree;
class CScope;
struct CFunctionBlock;


class CWhileStatementLinter final : public CStatementLinter, protected IRuntimeBlock
{
	NONCOPYABLE(CWhileStatementLinter);

public:
	explicit CWhileStatementLinter(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const owner);
	~CWhileStatementLinter();
	[[nodiscard]] Success Parse();

	[[nodiscard]] RuntimeBlock ToRuntimeObject() const override;

private:

	std::unique_ptr<AbstractSyntaxTree> m_pCondition;
};