#pragma once
#include "manager.hpp"

#include <unordered_map>

class CMemory;
class IConstEvalValue;

struct CLinterVariable final : public CMemoryIdentifier
{
	NONCOPYABLE(CLinterVariable);

	CLinterVariable(const CMemory* owner, const std::string& name, const CCrossModuleReference& ref);
	~CLinterVariable();

	[[nodiscard]] virtual constexpr EMemoryIdentifierType Type() const noexcept override { return mi_variable; }
	[[nodiscard]] bool IsGlobal() const noexcept;

	const CMemory* m_pOwner{};
	bool m_bCaptured{ false }; //captured by a closure (only lambdas for now)
	bool m_bParameter{ false };
	bool m_bConst{ false };
	bool m_bInitialized{ false };
#ifdef OPTIMIZATIONS
	bool m_bIsConstEval{ true }; //can be evaluated during linting
	IConstEvalValue* m_pConstEval{ nullptr };
#endif
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
	[[nodiscard]] CLinterVariable* GetVariableByIndex(std::size_t i) const;

    [[nodiscard]] std::size_t GetVariableCount() const noexcept;
    [[nodiscard]] auto& GetVariableIterator() { return m_oVariables; }

private:
    std::unordered_map<std::string, std::unique_ptr<CLinterVariable>> m_oVariables;
	CMemory* const m_pOwner;
};
