
#include "statement.hpp"
#include "linter/linter.hpp"
#include "linter/token.hpp"
#include "linter/error.hpp"
#include "linter/expressions/expression.hpp"
#include "linter/expressions/ast.hpp"
#include "linter/scopes/scope.hpp"
#include "linter/modules/module.hpp"

#include <cassert>

using namespace Varjus;

CStatementLinter::CStatementLinter(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const stack)
	: CLinterSingle(pos, end), m_pScope(scope), m_pOwner(stack) {

	assert(m_iterPos != m_iterEnd);
	CreateThisScope();
}
void CStatementLinter::CreateThisScope()
{
	if (const auto s = m_pScope.lock()) {
		m_pThisScope = s->CreateScope();
	} else {
		m_pOwner->GetModule()->PushError(VSL("!(const auto scope = m_pScope.lock())"), (*m_iterPos)->m_oSourcePosition);
		return;
	}
}
Varjus::Success CStatementLinter::ParseIdentifier(TokenType tt)
{
	if (IsEndOfBuffer() || (*m_iterPos)->Type() != tt) {
		m_pOwner->GetModule()->PushError(Varjus::fmt::format(VSL("expected \"{}\""), tokenTypeStrings[tt]),
			GetIteratorSafe()->m_oSourcePosition);
		return failure;
	}

	std::advance(m_iterPos, 1); 

	return success;
}

ASTNode CStatementLinter::ParseExpression()
{
	if (IsEndOfBuffer() || !(*m_iterPos)->IsOperator(p_par_open)) {
		m_pOwner->GetModule()->PushError(VSL("expected a \"(\""), GetIteratorSafe()->m_oSourcePosition);
		return nullptr;
	}

	std::advance(m_iterPos, 1); // skip (

	CLinterExpression expr(m_iterPos, m_iterEnd, m_pScope, m_pOwner);

	ASTNode&& temp = nullptr;

	if (expr.Parse(PairMatcher(p_par_open))) {
		temp = expr.ToMergedAST();
	}

	return temp;
}


Varjus::Success CStatementLinter::ParseScope()
{

	if (IsEndOfBuffer()) {
		m_pOwner->GetModule()->PushError(VSL("expected a \"{\" or an expression"), (*std::prev(m_iterPos))->m_oSourcePosition);
		return failure;
	}

	CLinterContext ctx{
		.m_iterPos = m_iterPos,
		.m_iterEnd = m_iterEnd,
		.scope = m_pThisScope,
		.memory = m_pOwner,
		.m_pModule = m_pOwner->GetModule(),
		.m_bAddInstructions = !m_pOwner->IsHoisting()
	};

	// no curlybracket? only add one instruction :)
	if (!(*m_iterPos)->IsOperator(p_curlybracket_open)) {
		return CBufferLinter::LintToken(ctx);
	}

	auto scope = CScopeLinter(m_iterPos, m_iterEnd, m_pThisScope, m_pOwner);
	if (!scope.Parse()) {
		return failure;
	}

	return success;
}