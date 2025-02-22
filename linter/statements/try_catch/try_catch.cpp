#include "try_catch.hpp"

#include "linter/token.hpp"
#include "linter/error.hpp"
#include "linter/punctuation.hpp"
#include "linter/expressions/expression.hpp"
#include "linter/expressions/ast.hpp"
#include "linter/scopes/scope.hpp"
#include "linter/functions/function.hpp"

#include "api/internal/globalDefinitions.hpp"

#include <cassert>

CTryCatchStatementLinter::CTryCatchStatementLinter(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const stack)
	: CStatementLinter(pos, end, scope, stack) {

	assert(m_iterPos != m_iterEnd);
}
CTryCatchStatementLinter::~CTryCatchStatementLinter() = default;

Success CTryCatchStatementLinter::Parse()
{

	if (!ParseIdentifier(TokenType::tt_try))
		return failure;

	if (!ParseScope())
		return failure;

	m_oTryInstructions = m_pThisScope->MoveInstructions();
	std::advance(m_iterPos, 1);
	return ParseCatchStatement();
}

Success CTryCatchStatementLinter::ParseCatchStatement()
{
	if (!ParseIdentifier(TokenType::tt_catch))
		return failure;

	CreateThisScope(); //recreate the scope
	
	if (IsEndOfBuffer() || !(*m_iterPos)->IsOperator(p_par_open)) {
		CLinterErrors::PushError("expected a \"(\"", GetIteratorSafe()->m_oSourcePosition);
		return failure;
	}

	std::advance(m_iterPos, 1);

	if (IsEndOfBuffer() || (*m_iterPos)->Type() != tt_name) {
		CLinterErrors::PushError("expected an identifier", GetIteratorSafe()->m_oSourcePosition);
		return failure;
	}

	if (!m_pThisScope->DeclareVariable((*m_iterPos)->Source())) {
		CLinterErrors::PushError("variable " + (*m_iterPos)->Source() + " already declared", 
			(*m_iterPos)->m_oSourcePosition);
		return failure;
	}

	assert(!m_pThisScope->IsGlobalScope());
	auto var = m_pOwner->m_VariableManager->DeclareVariable((*m_iterPos)->Source());
	assert(var);
	m_uCatchVariable = *var;

	std::advance(m_iterPos, 1);
	if (IsEndOfBuffer() || !(*m_iterPos)->IsOperator(p_par_close)) {
		CLinterErrors::PushError("expected a \")\"", GetIteratorSafe()->m_oSourcePosition);
		return failure;
	}
	std::advance(m_iterPos, 1); //skip )

	if (!ParseScope())
		return failure;

	m_oCatchInstructions = m_pThisScope->MoveInstructions();
	return success;
}

RuntimeBlock CTryCatchStatementLinter::ToRuntimeObject() const 
{
	using NonConstType = InstructionSequence&&;

	auto&& tryInsn = const_cast<NonConstType>(std::move(m_oTryInstructions));
	auto&& catchInsn = const_cast<NonConstType>(std::move(m_oCatchInstructions));

	return std::make_unique<CRuntimeTryCatchStatement>(m_uCatchVariable, std::move(tryInsn), std::move(catchInsn));
}