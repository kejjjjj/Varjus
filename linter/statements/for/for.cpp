#include "for.hpp"

#include "linter/token.hpp"
#include "linter/error.hpp"
#include "linter/punctuation.hpp"
#include "linter/expressions/expression.hpp"
#include "linter/expressions/ast.hpp"
#include "linter/scopes/scope.hpp"
#include "linter/declarations/variable_declarations.hpp"

#include "globalDefinitions.hpp"

#include <cassert>

CForStatementLinter::CForStatementLinter(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const stack)
	: CStatementLinter(pos, end, scope, stack) {

	assert(m_iterPos != m_iterEnd);
}
CForStatementLinter::~CForStatementLinter() = default;

Success CForStatementLinter::Parse()
{
	m_pThisScope->MakeLoopScope();

	if (!ParseIdentifier(TokenType::tt_for))
		return failure;

	if (IsEndOfBuffer() || !(*m_iterPos)->IsOperator(p_par_open)) {
		CLinterErrors::PushError("expected a \"(\"", GetIteratorSafe()->m_oSourcePosition);
		return failure;
	}

	std::advance(m_iterPos, 1);

	if (!ParseInitializer())
		return failure;

	if (!ParseCondition())
		return failure;

	if (!ParseEndExpression())
		return failure;

	return ParseScope();
}

Success CForStatementLinter::ParseInitializer() {

	if (IsEndOfBuffer()) {
		CLinterErrors::PushError("expected an expression", GetIteratorSafe()->m_oSourcePosition);
		return failure;
	}

	if ((*m_iterPos)->Type() == TokenType::tt_let) {
		CVariableDeclarationLinter linter(m_iterPos, m_iterEnd, m_pThisScope, m_pOwner);

		if (!linter.Parse())
			return failure;

		m_pInitializer = linter.MoveInitializer();
		std::advance(m_iterPos, 1);
		return success;
	}

	if ((*m_iterPos)->IsOperator(p_semicolon)) {
		std::advance(m_iterPos, 1);
		return success;
	}

	
	CLinterExpression linter(m_iterPos, m_iterEnd, m_pThisScope, m_pOwner);
	if (!linter.Parse())
		return failure;

	m_pInitializer = linter.ToMergedAST();
	std::advance(m_iterPos, 1);
	return success;
}
Success CForStatementLinter::ParseCondition() {

	if (IsEndOfBuffer()) {
		CLinterErrors::PushError("expected an expression", GetIteratorSafe()->m_oSourcePosition);
		return failure;
	}

	if ((*m_iterPos)->IsOperator(p_semicolon)) {
		std::advance(m_iterPos, 1);
		return success;
	}
	CLinterExpression linter(m_iterPos, m_iterEnd, m_pThisScope, m_pOwner);
	if (!linter.Parse())
		return failure;

	m_pCondition = linter.ToMergedAST();
	std::advance(m_iterPos, 1);
	return success;
}
Success CForStatementLinter::ParseEndExpression() {
	if (IsEndOfBuffer()) {
		CLinterErrors::PushError("expected an expression", GetIteratorSafe()->m_oSourcePosition);
		return failure;
	}

	if ((*m_iterPos)->IsOperator(p_par_close)) {
		std::advance(m_iterPos, 1);
		return success;
	}

	CLinterExpression linter(m_iterPos, m_iterEnd, m_pThisScope, m_pOwner);
	if (!linter.Parse(PairMatcher(p_par_open))) // match the )
		return failure;

	m_pOnEnd = linter.ToMergedAST();
	return success;
}

RuntimeBlock CForStatementLinter::ToRuntimeObject() const
{
	decltype(auto) init = const_cast<ASTNode&&>(std::move(m_pInitializer));
	decltype(auto) cond = const_cast<ASTNode&&>(std::move(m_pCondition));
	decltype(auto) endExpr = const_cast<ASTNode&&>(std::move(m_pOnEnd));

	return std::make_unique<CRuntimeForStatement>(
		std::move(init),
		std::move(cond),
		std::move(endExpr),
		m_pThisScope->MoveInstructions()
	);
}