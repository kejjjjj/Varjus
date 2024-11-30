#include "scope.hpp"

#include "linter/token.hpp"
#include "linter/punctuation.hpp"
#include "linter/linter.hpp"
#include "linter/error.hpp"

#include "runtime/structure.hpp"

#include "globalEnums.hpp"

#include <cassert>

CScopeLinter::CScopeLinter(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const owner)
	: CLinterSingle(pos, end), m_pScope(scope), m_pOwner(owner) {

	assert(!IsEndOfBuffer() && (*m_iterPos)->IsOperator(p_curlybracket_open));
}

Success CScopeLinter::Parse()
{
	assert(!IsEndOfBuffer() && (*m_iterPos)->IsOperator(p_curlybracket_open));
	std::advance(m_iterPos, 1);

	do {
		if (!CFileLinter::LintToken(m_iterPos, m_iterEnd, m_pScope, m_pOwner))
			break;

		std::advance(m_iterPos, 1);

	} while (!IsEndOfBuffer() && !(*m_iterPos)->IsOperator(p_curlybracket_close));

	if (IsEndOfBuffer())
		return success;

	return success;
}
CScope::CScope(CMemory* const owner) : m_pOwner(owner){}
std::shared_ptr<CScope> CScope::CreateScope()
{
	auto scope = std::make_shared<CScope>(m_pOwner);
	scope->m_pLowerScope = this;
	
	if (m_bIsWithinLoop)
		scope->MakeLoopScope();

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

	if (IsGlobalScope())
		return false;

	return m_pLowerScope->VariableExists(var);
}

bool CScope::IsLoopScope() const noexcept
{
	return m_bIsWithinLoop;
}
void CScope::AddInstruction(RuntimeBlock&& block)
{
	m_oInstructions.emplace_back(std::move(block));
}
VectorOf<RuntimeBlock>&& CScope::MoveInstructions()
{
	return std::move(m_oInstructions);
}
IRuntimeStructure* CScope::GetLatestInstruction() const noexcept
{
	return m_oInstructions.size() ? &*m_oInstructions.back() : nullptr;
}
