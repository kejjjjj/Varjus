#pragma once

#include "linter/expressions/definitions.hpp"
#include "linter/declarations/stack.hpp"
#include "runtime/structure.hpp"

enum TokenType : signed char;

class CScope;
class AbstractSyntaxTree;

class CStatementParser : public CLinterSingle<CToken>
{
	NONCOPYABLE(CStatementParser);

public:
	explicit CStatementParser(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const owner);

protected:

	[[nodiscard]] Success ParseIdentifier(TokenType tt);
	[[nodiscard]] virtual std::unique_ptr<AbstractSyntaxTree> ParseExpression();
	[[nodiscard]] virtual Success ParseScope();
	
	WeakScope m_pScope;
	CMemory* const m_pOwner;

	std::shared_ptr<CScope> m_pThisScope;
};
