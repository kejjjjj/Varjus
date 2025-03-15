#pragma once
#include "manager.hpp"

#include <unordered_map>

class CMemory;
class CConstEvalVariable;

struct CLinterVariable : public CMemoryIdentifier
{
	NONCOPYABLE(CLinterVariable);

	CLinterVariable(const CMemory* owner, const VarjusString& name, const CCrossModuleReference& ref);
	virtual ~CLinterVariable();

	[[nodiscard]] virtual constexpr EMemoryIdentifierType Type() const noexcept override { return mi_variable; }
	[[nodiscard]] bool IsGlobal() const noexcept;


	const CMemory* m_pOwner{};
	bool m_bCaptured{ false }; //captured by a closure (only lambdas for now)
	bool m_bParameter{ false };
	bool m_bConst{ false };
	bool m_bInitialized{ false };

};

template <typename T1, typename T2 = void>
class CVariableManager
{
	NONCOPYABLE(CVariableManager);
	friend class CMemory;
public:
	CVariableManager(CMemory* const m_pOwner);

    [[maybe_unused]] T1* DeclareVariable(const VarjusString& var);
    [[nodiscard]] T1* GetVariable(const VarjusString& var);
    [[nodiscard]] bool ContainsVariable(const VarjusString& name) const;
	[[nodiscard]] T1* GetVariableByIndex(std::size_t i) const;

    [[nodiscard]] std::size_t GetVariableCount() const noexcept;
    [[nodiscard]] auto& GetVariableIterator() { return m_oVariables; }

private:
    std::unordered_map<VarjusString, std::unique_ptr<T1>> m_oVariables;

	CMemory* const m_pOwner;
};
