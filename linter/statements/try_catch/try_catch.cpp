#include "try_catch.hpp"

#include "linter/token.hpp"
#include "linter/error.hpp"
#include "linter/punctuation.hpp"
#include "linter/expressions/expression.hpp"
#include "linter/expressions/ast.hpp"
#include "linter/scopes/scope.hpp"
#include "linter/functions/function.hpp"
#include "linter/modules/module.hpp"

#include "varjus_api/internal/globalDefinitions.hpp"

#include <cassert>

using namespace Varjus;

CTryCatchStatementLinter::CTryCatchStatementLinter(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const stack)
	: CStatementLinter(pos, end, scope, stack) {

	assert(m_iterPos != m_iterEnd);
}
CTryCatchStatementLinter::~CTryCatchStatementLinter() = default;

Varjus::Success CTryCatchStatementLinter::Parse()
{

	if (!ParseIdentifier(TokenType::tt_try))
		return failure;

	if (!ParseScope())
		return failure;

	m_oTryInstructions = m_pThisScope->MoveInstructions();
	std::advance(m_iterPos, 1);
	return ParseCatchStatement();
}

Varjus::Success CTryCatchStatementLinter::ParseCatchStatement()
{
	if (!ParseIdentifier(TokenType::tt_catch))
		return failure;

	CreateThisScope(); //recreate the scope
	
	if (IsEndOfBuffer() || !(*m_iterPos)->IsOperator(p_par_open)) {
		m_pOwner->GetModule()->PushError(VSL("expected a \"(\""), GetIteratorSafe()->m_oSourcePosition);
		return failure;
	}

	std::advance(m_iterPos, 1);

	if (IsEndOfBuffer() || (*m_iterPos)->Type() != tt_name) {
		m_pOwner->GetModule()->PushError(VSL("expected an identifier"), GetIteratorSafe()->m_oSourcePosition);
		return failure;
	}

	if (!m_pThisScope->DeclareVariable((*m_iterPos)->Source())) {
		m_pOwner->GetModule()->PushError(VSL("variable ") + (*m_iterPos)->Source() + VSL(" already declared"),
			(*m_iterPos)->m_oSourcePosition);
		return failure;
	}

	assert(!m_pThisScope->IsGlobalScope());
	auto var = m_pOwner->m_VariableManager->DeclareVariable((*m_iterPos)->Source());
	assert(var);
	m_uCatchVariable.m_bBelongsToDifferentModule = var->m_bBelongsToDifferentModule;
	m_uCatchVariable.m_uIndex = var->m_uIndex;
	m_uCatchVariable.m_uModuleIndex = var->m_uModuleIndex;

	std::advance(m_iterPos, 1);
	if (IsEndOfBuffer() || !(*m_iterPos)->IsOperator(p_par_close)) {
		m_pOwner->GetModule()->PushError(VSL("expected a \")\""), GetIteratorSafe()->m_oSourcePosition);
		return failure;
	}
	std::advance(m_iterPos, 1); //skip )

	if (!ParseScope())
		return failure;

	m_oCatchInstructions = m_pThisScope->MoveInstructions();
	return success;
}

__RuntimeBlock CTryCatchStatementLinter::ToRuntimeObject() const 
{
	using NonConstType = __InstructionSequence&&;

	auto&& tryInsn = const_cast<NonConstType>(std::move(m_oTryInstructions));
	auto&& catchInsn = const_cast<NonConstType>(std::move(m_oCatchInstructions));

	return std::make_unique<CRuntimeTryCatchStatement>(m_uCatchVariable, std::move(tryInsn), std::move(catchInsn));
}