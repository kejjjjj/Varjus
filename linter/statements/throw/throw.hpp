#pragma once

#include "linter/statements/statement.hpp"

class IRuntimeStructure;
class AbstractSyntaxTree;
class CScope;
struct CFunctionBlock;


class CThrowStatementLinter final : public CStatementLinter, protected IRuntimeBlock
{
	NONCOPYABLE(CThrowStatementLinter);

public:
	explicit CThrowStatementLinter(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const owner);
	~CThrowStatementLinter();
	[[nodiscard]] Success Parse();

	[[nodiscard]] ASTNode ParseExpression() override;

	[[nodiscard]] RuntimeBlock ToRuntimeObject() const override;

private:

	ASTNode m_pAST;
};