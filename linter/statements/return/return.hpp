#pragma once

#include "linter/statements/statement.hpp"

class IRuntimeStructure;
class AbstractSyntaxTree;
class CScope;
struct CFunctionBlock;


class CReturnStatementLinter final : public CStatementLinter, protected IRuntimeBlock
{
	NONCOPYABLE(CReturnStatementLinter);
public:
	explicit CReturnStatementLinter(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const owner);
	~CReturnStatementLinter();
	[[nodiscard]] Success Parse();

	[[nodiscard]] ASTNode ParseExpression() override;

	[[nodiscard]] RuntimeBlock ToRuntimeObject() const override;

private:

	ASTNode m_pAST;
};