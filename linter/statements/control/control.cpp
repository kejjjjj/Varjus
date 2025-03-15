#include "control.hpp"

#include "linter/token.hpp"
#include "linter/error.hpp"
#include "linter/punctuation.hpp"
#include "linter/expressions/expression.hpp"
#include "linter/expressions/ast.hpp"
#include "linter/scopes/scope.hpp"
#include "linter/modules/module.hpp"

#include "varjus_api/internal/globalDefinitions.hpp"

#include <cassert>


CLoopControlStatement::CLoopControlStatement(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const stack)
	: CStatementLinter(pos, end, scope, stack) {

	assert(m_iterPos != m_iterEnd);
}
CLoopControlStatement::~CLoopControlStatement() = default;

Success CLoopControlStatement::Parse()
{

	assert(!IsEndOfBuffer());

	if ((*m_iterPos)->Type() == tt_break) {
		m_eType = lc_break;
	}else if ((*m_iterPos)->Type() == tt_continue) {
		m_eType = lc_continue;
	} else {
		m_pOwner->GetModule()->PushError(VSL("expected \"break\" or \"continue\""), GetIteratorSafe()->m_oSourcePosition);
		return failure;
	}

	std::array<VarjusString, 3> quick_lookup = {{ VSL("null"), VSL("break"), VSL("continue")}};

	if (const auto scope = m_pScope.lock()) {
		if (!scope->IsLoopScope()) {
			m_pOwner->GetModule()->PushError(
				fmt::format(VSL("the \"{}\" statement can only be used in a loop context"), quick_lookup[(std::size_t)m_eType]),
				GetIteratorSafe()->m_oSourcePosition);
			return failure;
		}
	}
	else {
		m_pOwner->GetModule()->PushError(VSL("!(const auto scope = m_pScope.lock())"), GetIteratorSafe()->m_oSourcePosition);
		return failure;
	}
	
	std::advance(m_iterPos, 1);

	return success;
}

RuntimeBlock CLoopControlStatement::ToRuntimeObject() const{
	return std::make_unique<CRuntimeLoopControlStatement>(m_eType);
}