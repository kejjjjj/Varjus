#pragma once

#include "definitions.hpp"
#include "varjus_api/internal/globalDefinitions.hpp"

class CMemory;
class CScope;
struct CMemoryIdentifier;
struct CLinterVariable;
struct CLinterFunction;

class CIdentifierLinter final : public CLinterSingle<CToken>
{
	friend class CLinterOperand;
public:
	CIdentifierLinter() = delete;
	explicit CIdentifierLinter(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const stack);
	~CIdentifierLinter();

	[[nodiscard]] Varjus::Success ParseIdentifier();

private:
	[[nodiscard]] bool CheckIdentifier(const CToken* token) const noexcept;

	[[nodiscard]] bool ContainsFunction(const VarjusString& str) const noexcept;


	template<typename T>
	[[nodiscard]] CLinterVariable* GetVariableByIdentifier(const VarjusString& str) const noexcept;

	[[nodiscard]] CLinterFunction* GetFunctionByIdentifier(const VarjusString& str) const noexcept;


	WeakScope m_pScope;
	CMemory* const m_pOwner;
	CMemoryIdentifier* m_pIdentifier{ nullptr };
};