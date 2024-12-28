#pragma once
#include "manager.hpp"

#include <unordered_map>

class CMemory;

struct CLinterVariable final : public CMemoryIdentifier
{
	CLinterVariable() = delete;
	CLinterVariable(const CMemory* owner, const std::string& name, const CCrossModuleReference& ref)
		: CMemoryIdentifier(name, ref), m_pOwner(owner) {}

	[[nodiscard]] virtual constexpr EMemoryIdentifierType Type() const noexcept override { return mi_variable; }
	[[nodiscard]] bool IsGlobal() const noexcept;

	const CMemory* m_pOwner{};
	bool m_bCaptured{ false }; //captured by a closure (only lambdas for now)
	bool m_bParameter{ false };
	bool m_bConst{ false };
	bool m_bInitialized{ false };
};

class CVariableManager
{
	NONCOPYABLE(CVariableManager);
	friend class CMemory;
public:
	CVariableManager(CMemory* const m_pOwner);

    [[maybe_unused]] CLinterVariable* DeclareVariable(const std::string& var);
    [[nodiscard]] CLinterVariable* GetVariable(const std::string& var);
    [[nodiscard]] bool ContainsVariable(const std::string& name) const;
    [[nodiscard]] std::size_t GetVariableCount() const noexcept;
    [[nodiscard]] auto& GetVariableIterator() { return m_oVariables; }

private:
    std::unordered_map<std::string, std::unique_ptr<CLinterVariable>> m_oVariables;
	CMemory* const m_pOwner;
};
