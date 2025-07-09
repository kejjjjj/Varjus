#include "else.hpp"
#include "if.hpp"

#include "linter/token.hpp"
#include "linter/error.hpp"
#include "linter/punctuation.hpp"
#include "linter/expressions/expression.hpp"
#include "linter/expressions/ast.hpp"
#include "linter/scopes/scope.hpp"
#include "linter/modules/module.hpp"

#include "varjus_api/internal/globalDefinitions.hpp"

#include <cassert>

using namespace Varjus;

CElseStatementLinter::CElseStatementLinter(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const stack)
	: CStatementLinter(pos, end, scope, stack) {

	if (const auto oldScope = m_pScope.lock()) {
		m_pPreviousBlock = dynamic_cast<CRuntimeConditionalStatement*>(oldScope->GetLatestInstruction());
	} else {
		m_pOwner->GetModule()->PushError(VSL("const auto oldScope = m_pScope.lock()"), GetIteratorSafe()->m_oSourcePosition);
	}

	assert(m_iterPos != m_iterEnd);
}
CElseStatementLinter::~CElseStatementLinter() = default;

Varjus::Success CElseStatementLinter::Parse()
{
	if (!m_pOwner->IsHoisting() && !IsInConditionalContext()) {
		m_pOwner->GetModule()->PushError(VSL("an else block must be after an if block"),
			GetIteratorSafe()->m_oSourcePosition);
		return failure;
	}

	if (!ParseIdentifier(TokenType::tt_else))
		return failure;

	if (IsEndOfBuffer()) {
		m_pOwner->GetModule()->PushError(VSL("expected a \"{\" or an expression"), GetIteratorSafe()->m_oSourcePosition);
		return failure;
	}

	//this data is unavailable during hoisting
	auto lastBlock = m_pOwner->IsHoisting() ? nullptr : m_pPreviousBlock->SeekLastBlock();
	
	//else if?
	if ((*m_iterPos)->Type() == tt_if) {

		auto ifStatement = CIfStatementLinter(m_iterPos, m_iterEnd, m_pThisScope, m_pOwner);

		if (!ifStatement.Parse())
			return failure;

		if (m_pOwner->IsHoisting())
			return success;

		//else if
		if(!m_pOwner->IsHoisting())
			lastBlock->m_pNext = ifStatement.ToConditionalObject();

		return success;
	}

	if (!ParseScope())
		return failure;

	if (m_pOwner->IsHoisting())
		return success;

	//else block
	if(!m_pOwner->IsHoisting())
		lastBlock->m_pNext = std::make_unique<CRuntimeConditionalStatement>(nullptr, m_pThisScope->MoveInstructions());
		
	return success;
}

bool CElseStatementLinter::IsInConditionalContext() const
{
	return m_pPreviousBlock ? m_pPreviousBlock->Type() == st_conditional : false;
}

__RuntimeBlock CElseStatementLinter::ToRuntimeObject() const
{
	assert(false);
	return nullptr;
}