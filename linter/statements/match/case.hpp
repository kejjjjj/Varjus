#pragma once

#include "linter/statements/statement.hpp"

class IRuntimeStructure;
class AbstractSyntaxTree;
class CScope;
struct CFunctionBlock;


class CCaseStatementLinter final : public CStatementLinter, protected IRuntimeBlock
{
	VARJUS_NONCOPYABLE(CCaseStatementLinter);

public:
	explicit CCaseStatementLinter(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const owner);
	~CCaseStatementLinter();
	[[nodiscard]] Varjus::Success Parse();

	[[nodiscard]] __RuntimeBlock ToRuntimeObject() const override;

private:
	[[nodiscard]] Varjus::Success ParseCaseScope();
	[[nodiscard]] Varjus::Success ParseDefaultClause();

	[[nodiscard]] bool ContainsDefaultClause() const noexcept;

	ASTNode m_pExpression;
	std::unordered_map<ASTNode, __RuntimeBlock> m_oCases;
	__RuntimeBlock m_oDefaultCase;
};