#pragma once

#include "definitions.hpp"
#include "globalDefinitions.hpp"

class CMemory;
class CScope;
struct CMemoryIdentifier;

class CIdentifierLinter final : public CLinterSingle<CToken>
{
	friend class CLinterOperand;
public:
	CIdentifierLinter() = delete;
	explicit CIdentifierLinter(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const stack);
	~CIdentifierLinter();

	[[nodiscard]] Success ParseIdentifier();
	[[nodiscard]] auto GetToken() const { return m_pToken; }

	[[nodiscard]] bool IsImmediate() const noexcept;
	[[nodiscard]] EValueType GetImmediateType() const noexcept;
	[[nodiscard]] std::size_t GetImmediateSize() const noexcept;
	[[nodiscard]] std::string ToData() const noexcept;

private:
	[[nodiscard]] bool CheckIdentifier(const CToken* token) const noexcept;

	WeakScope m_pScope;
	CMemory* const m_pOwner;
	CMemoryIdentifier* m_pIdentifier;
};