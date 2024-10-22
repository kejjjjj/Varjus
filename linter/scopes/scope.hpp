#pragma once

#include "linter/expressions/definitions.hpp"
#include "linter/declarations/stack.hpp"
#include <unordered_set>

struct CLinterVariable;
class CScope;

class CScopeLinter final
{
	NONCOPYABLE(CScopeLinter);
public:
	explicit CScopeLinter(LinterIterator& pos, LinterIterator& end, CMemory* const owner);

	[[nodiscard]] Success ParseScope();

private:
	[[nodiscard]] constexpr bool IsEndOfBuffer() const noexcept { return m_iterPos == m_iterEnd; }

	LinterIterator& m_iterPos;
	LinterIterator& m_iterEnd;
	CMemory* const m_pOwner;
	std::weak_ptr<CScope> m_pScope;
};

class CScope final
{
	NONCOPYABLE(CScope);
public:
	CScope() = delete;
	CScope(CMemory* const owner);

	[[nodiscard]] constexpr bool IsGlobalScope() { return !m_pLowerScope; }
	[[nodiscard]] std::shared_ptr<CScope> CreateScope();

	[[nodiscard]] static CScope* DeleteScope(CScope* scope);

	[[nodiscard]] Success DeclareVariable(const std::string& var);
private:
	[[nodiscard]] bool VariableExists(const std::string& var);

	CMemory* const m_pOwner;
	CScope* m_pLowerScope{};
	std::unordered_set<std::string> m_oLocalVariables;
};