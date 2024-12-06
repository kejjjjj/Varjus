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

	[[nodiscard]] std::unique_ptr<AbstractSyntaxTree> ParseExpression() override;

	[[nodiscard]] RuntimeBlock ToRuntimeObject() const override;

private:

	std::unique_ptr<AbstractSyntaxTree> m_pAST;
};