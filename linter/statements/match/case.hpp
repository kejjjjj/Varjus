#pragma once

#include "linter/statements/statement.hpp"

class IRuntimeStructure;
class AbstractSyntaxTree;
class CScope;
struct CFunctionBlock;


class CCaseStatementLinter final : public CStatementLinter, protected IRuntimeBlock
{
	NONCOPYABLE(CCaseStatementLinter);

public:
	explicit CCaseStatementLinter(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const owner);
	~CCaseStatementLinter();
	[[nodiscard]] Success Parse();

	[[nodiscard]] RuntimeBlock ToRuntimeObject() const override;

private:
	[[nodiscard]] Success ParseCaseScope();
	[[nodiscard]] Success ParseDefaultClause();

	[[nodiscard]] bool ContainsDefaultClause() const noexcept;

	ASTNode m_pExpression;
	std::unordered_map<ASTNode, RuntimeBlock> m_oCases;
	RuntimeBlock m_oDefaultCase;
};