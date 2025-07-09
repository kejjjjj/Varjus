#pragma once

#include "linter/expressions/definitions.hpp"
#include "linter/functions/stack.hpp"
#include "varjus_api/internal/structure.hpp"

enum TokenType : signed char;

class CScope;
class AbstractSyntaxTree;

class CStatementLinter : public CLinterSingle<CToken>
{
	VARJUS_NONCOPYABLE(CStatementLinter);

public:
	explicit CStatementLinter(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const owner);

protected:
	void CreateThisScope();

	[[nodiscard]] Varjus::Success ParseIdentifier(TokenType tt);
	[[nodiscard]] virtual ASTNode ParseExpression();
	[[nodiscard]] virtual Varjus::Success ParseScope();

	WeakScope m_pScope;
	CMemory* const m_pOwner;

	std::shared_ptr<CScope> m_pThisScope;
};
