#pragma once

#include "linter/expressions/definitions.hpp"
#include "linter/functions/stack.hpp"
#include <unordered_set>

struct CLinterVariable;
class CScope;
class IRuntimeStructure;

class CScopeLinter final : public CLinterSingle<CToken>
{
	VARJUS_NONCOPYABLE(CScopeLinter);
public:
	explicit CScopeLinter(LinterIterator& pos, LinterIterator& end, const WeakScope& s, CMemory* const owner);

	[[maybe_unused]] Varjus::Success Parse();
	[[maybe_unused]] Varjus::Success ParseUntil(bool(*test)(LinterIterator& i));

private:

	WeakScope m_pScope;
	CMemory* const m_pOwner;

	VectorOf<std::unique_ptr<IRuntimeStructure>> m_oInstructions; // sorted list of all function instructions
};

class CScope final
{
	VARJUS_NONCOPYABLE(CScope);
	friend class CIdentifierLinter;
public:
	CScope() = delete;
	CScope(CMemory* const owner);
	~CScope();

	[[nodiscard]] constexpr bool IsGlobalScope() const noexcept { return !m_pLowerScope; }
	[[nodiscard]] std::shared_ptr<CScope> CreateScope();

	[[nodiscard]] static CScope* DeleteScope(CScope* scope);

	[[nodiscard]] Varjus::Success DeclareVariable(const VarjusString& var);
	[[nodiscard]] bool VariableExists(const VarjusString& var) const;

	constexpr void MakeLoopScope(bool s = true) noexcept { m_bIsWithinLoop = s; }
	[[nodiscard]] constexpr bool IsLoopScope() const noexcept { return m_bIsWithinLoop; }

	constexpr void MakeMatchScope(bool s = true) noexcept { m_bIsMatchStatement = s; }
	[[nodiscard]] constexpr bool IsMatchScope() const noexcept { return m_bIsMatchStatement; }

	void AddInstruction(__RuntimeBlock&& block);
	void AddInstructions(VectorOf<__RuntimeBlock>&& block);

	[[nodiscard]] VectorOf<__RuntimeBlock>&& MoveInstructions();
	[[nodiscard]] IRuntimeStructure* GetLatestInstruction() const noexcept;

	[[nodiscard]] constexpr auto& GetInstructions() const noexcept { return m_oInstructions; }

private:

	VectorOf<__RuntimeBlock> m_oInstructions;

	CMemory* const m_pOwner;
	CScope* m_pLowerScope{};
	std::unordered_set<VarjusString> m_oLocalVariables;
	bool m_bIsWithinLoop{ false };
	bool m_bIsMatchStatement{ false };
};