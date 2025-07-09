#pragma once

#include "linter/statements/statement.hpp"

class IRuntimeStructure;
class AbstractSyntaxTree;
class CScope;
struct CFunctionBlock;

class CIfStatementLinter final : public CStatementLinter, protected IRuntimeBlock
{
	VARJUS_NONCOPYABLE(CIfStatementLinter);

public:
	explicit CIfStatementLinter(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const owner);
	~CIfStatementLinter();
	[[nodiscard]] Varjus::Success Parse();

	[[nodiscard]] RuntimeBlock ToRuntimeObject() const override;
	[[nodiscard]] std::unique_ptr<CRuntimeConditionalStatement> ToConditionalObject();
private:

	ASTNode m_pCondition;
};