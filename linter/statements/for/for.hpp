#pragma once

#include "linter/statements/statement.hpp"

class IRuntimeStructure;
class AbstractSyntaxTree;
class CScope;
struct CFunctionBlock;


class CForStatementLinter final : public CStatementLinter, protected IRuntimeBlock
{
	NONCOPYABLE(CForStatementLinter);

public:
	explicit CForStatementLinter(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const owner);
	~CForStatementLinter();
	[[nodiscard]] Success Parse();

	[[nodiscard]] RuntimeBlock ToRuntimeObject() const override;

private:

	[[nodiscard]] Success ParseInitializer();
	[[nodiscard]] Success ParseCondition();
	[[nodiscard]] Success ParseEndExpression();


	ASTNode m_pInitializer;
	ASTNode m_pCondition;
	ASTNode m_pOnEnd;
};