#pragma once

#include "linter/statements/statement.hpp"

class IRuntimeStructure;
class AbstractSyntaxTree;
class CScope;
struct CFunctionBlock;

class CIfStatementLinter final : public CStatementLinter, protected IRuntimeBlock
{
	NONCOPYABLE(CIfStatementLinter);

public:
	explicit CIfStatementLinter(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const owner);
	~CIfStatementLinter();
	[[nodiscard]] Success Parse();

	[[nodiscard]] RuntimeBlock ToRuntimeObject() const override;
	[[nodiscard]] std::unique_ptr<CRuntimeConditionalStatement> ToConditionalObject();
private:

	std::unique_ptr<AbstractSyntaxTree> m_pCondition;
};