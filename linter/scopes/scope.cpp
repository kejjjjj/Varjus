#include "scope.hpp"

#include "linter/token.hpp"
#include "linter/punctuation.hpp"
#include "linter/linter.hpp"
#include "linter/error.hpp"

#include "runtime/structure.hpp"

#include "globalEnums.hpp"

#include <cassert>

CScopeLinter::CScopeLinter(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const owner)
	: m_iterPos(pos), m_iterEnd(end), m_pScope(scope), m_pOwner(owner) {

	assert(!IsEndOfBuffer() && (*m_iterPos)->IsOperator(p_curlybracket_open));
}

Success CScopeLinter::ParseScope()
{
	assert(!IsEndOfBuffer() && (*m_iterPos)->IsOperator(p_curlybracket_open));


	if (auto parentScope = m_pScope.lock()) {
		auto newScope = parentScope->CreateScope();
		
		std::advance(m_iterPos, 1);

		do {

			if (!CFileLinter::LintToken(m_iterPos, m_iterEnd, newScope, m_pOwner))
				break;

			std::advance(m_iterPos, 1);

		} while (!IsEndOfBuffer() && !(*m_iterPos)->IsOperator(p_curlybracket_close));

	} else {
		CLinterErrors::PushError("!auto parentScope = m_pScope.lock()", IsEndOfBuffer() ? (*std::prev(m_iterPos))->m_oSourcePosition : (*m_iterPos)->m_oSourcePosition);
		return failure;
	}

	if(IsEndOfBuffer() || !(*m_iterPos)->IsOperator(p_curlybracket_close))
		CLinterErrors::PushError("expected a \"}\"", IsEndOfBuffer() ? (*std::prev(m_iterPos))->m_oSourcePosition : (*m_iterPos)->m_oSourcePosition);

	std::advance(m_iterPos, 1);
	return success;
}
CScope::CScope(CMemory* const owner) : m_pOwner(owner){}
std::shared_ptr<CScope> CScope::CreateScope()
{
	auto scope = std::make_shared<CScope>(m_pOwner);
	scope->m_pLowerScope = this;
	return scope;
}

CScope* CScope::DeleteScope(CScope* scope)
{
	assert(scope);

	auto temp = scope->m_pLowerScope;

	delete scope;
	scope = nullptr;

	return temp;
}

Success CScope::DeclareVariable(const std::string& var)
{
	if (VariableExists(var))
		return failure;

	m_oLocalVariables.insert(var);
	return success;
}
bool CScope::VariableExists(const std::string& var) const
{
	const auto found = m_oLocalVariables.find(var) != m_oLocalVariables.end();

	if (found)
		return true;

	if (!IsGlobalScope())
		return false;

	return m_pLowerScope->VariableExists(var);
}
