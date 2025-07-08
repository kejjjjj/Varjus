#pragma once

#include "linter/statements/statement.hpp"

class IRuntimeStructure;
class AbstractSyntaxTree;
class CScope;
struct CFunctionBlock;


class CMatchStatementLinter final : public CStatementLinter, protected IRuntimeBlock
{
	VARJUS_NONCOPYABLE(CMatchStatementLinter);

public:
	explicit CMatchStatementLinter(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const owner);
	~CMatchStatementLinter();
	[[nodiscard]] Varjus::Success Parse();

	[[nodiscard]] RuntimeBlock ToRuntimeObject() const override;
	[[nodiscard]] CRuntimeCaseStatement* GetDefaultCase() const noexcept;
private:
	ASTNode m_pExpression;
};