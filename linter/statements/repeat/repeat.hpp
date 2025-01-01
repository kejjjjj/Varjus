#pragma once

#include "linter/statements/statement.hpp"

class IRuntimeStructure;
class AbstractSyntaxTree;
class CScope;
struct CFunctionBlock;


class CRepeatStatementLinter final : public CStatementLinter, protected IRuntimeBlock
{
	NONCOPYABLE(CRepeatStatementLinter);

public:
	explicit CRepeatStatementLinter(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const owner);
	~CRepeatStatementLinter();
	[[nodiscard]] Success Parse();

	[[nodiscard]] RuntimeBlock ToRuntimeObject() const override;

private:

	ASTNode m_pCondition;
};