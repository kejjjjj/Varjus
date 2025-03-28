#pragma once

#include "linter/expressions/definitions.hpp"
#include "linter/functions/stack.hpp"
#include <unordered_set>

struct CLinterVariable;
class CScope;
class IRuntimeStructure;

class CScopeLinter final : public CLinterSingle<CToken>
{
	NONCOPYABLE(CScopeLinter);
public:
	explicit CScopeLinter(LinterIterator& pos, LinterIterator& end, const WeakScope& s, CMemory* const owner);

	[[maybe_unused]] Success Parse();

private:

	WeakScope m_pScope;
	CMemory* const m_pOwner;

	VectorOf<std::unique_ptr<IRuntimeStructure>> m_oInstructions; // sorted list of all function instructions
};

class CScope final
{
	NONCOPYABLE(CScope);
	friend class CIdentifierLinter;
public:
	CScope() = delete;
	CScope(CMemory* const owner);
	~CScope();

	[[nodiscard]] constexpr bool IsGlobalScope() const noexcept { return !m_pLowerScope; }
	[[nodiscard]] std::shared_ptr<CScope> CreateScope();

	[[nodiscard]] static CScope* DeleteScope(CScope* scope);

	[[nodiscard]] Success DeclareVariable(const VarjusString& var);
	[[nodiscard]] bool VariableExists(const VarjusString& var) const;

	constexpr void MakeLoopScope(bool s = true) noexcept { m_bIsWithinLoop = s; }
	[[nodiscard]] bool IsLoopScope() const noexcept;

	void AddInstruction(RuntimeBlock&& block);
	void AddInstructions(VectorOf<RuntimeBlock>&& block);

	VectorOf<RuntimeBlock>&& MoveInstructions();

	IRuntimeStructure* GetLatestInstruction() const noexcept;

private:

	VectorOf<RuntimeBlock> m_oInstructions;

	CMemory* const m_pOwner;
	CScope* m_pLowerScope{};
	std::unordered_set<VarjusString> m_oLocalVariables;
	bool m_bIsWithinLoop{ false };
};