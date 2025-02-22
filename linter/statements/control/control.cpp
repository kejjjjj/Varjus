#include "control.hpp"

#include "linter/token.hpp"
#include "linter/error.hpp"
#include "linter/punctuation.hpp"
#include "linter/expressions/expression.hpp"
#include "linter/expressions/ast.hpp"
#include "linter/scopes/scope.hpp"

#include "api/internal/globalDefinitions.hpp"

#include <cassert>
#include <format>

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
		CLinterErrors::PushError("expected \"break\" or \"continue\"", GetIteratorSafe()->m_oSourcePosition);
		return failure;
	}

	constexpr std::array<const char*, 3> quick_lookup = {{ "null", "break", "continue"}};

	if (const auto scope = m_pScope.lock()) {
		if (!scope->IsLoopScope()) {
			CLinterErrors::PushError(
				std::format("the \"{}\" statement can only be used in a loop context", quick_lookup[(std::size_t)m_eType]),
				GetIteratorSafe()->m_oSourcePosition);
			return failure;
		}
	}
	else {
		CLinterErrors::PushError("!(const auto scope = m_pScope.lock())", GetIteratorSafe()->m_oSourcePosition);
		return failure;
	}
	
	std::advance(m_iterPos, 1);

	return success;
}

RuntimeBlock CLoopControlStatement::ToRuntimeObject() const{
	return std::make_unique<CRuntimeLoopControlStatement>(m_eType);
}