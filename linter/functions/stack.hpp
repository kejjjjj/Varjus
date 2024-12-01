#pragma once

#include <vector>
#include <memory>
#include <set>

#include "globalDefinitions.hpp"

#include "memory/manager_variable.hpp"
#include "memory/manager_function.hpp"

class CStack;
class IRuntimeStructure;
class CFileRuntimeData;

struct CFunctionBlock;
struct CProgramContext;

template<typename T>
using VectorOf = std::vector<T>;
using RuntimeBlock = std::unique_ptr<IRuntimeStructure>;


class CMemory
{
	NONCOPYABLE(CMemory);

	friend class CFunctionLinter;
	friend class CIdentifierLinter;
	friend class CFunctionManager;
	friend class CVariableDeclarationLinter;

public:
	CMemory(CMemory* globalMemory, CFileRuntimeData* const file, CProgramContext* const context);
	virtual ~CMemory();

	[[nodiscard]] virtual bool IsStack() const noexcept { return false; }

	[[nodiscard]] CStack* ToStack();
	[[nodiscard]] auto ToStack() const;

	[[nodiscard]] constexpr CProgramContext* GetContext() const { return m_pContext; }
	[[nodiscard]] auto& GetGlobalMemory() noexcept { return m_pGlobal; }
	[[nodiscard]] auto& GetGlobalMemory() const noexcept { return m_pGlobal; }

	[[nodiscard]] bool IsGlobalMemory() const noexcept { return this == m_pGlobal; }
	[[nodiscard]] auto& GetLowerMemoryRegion() const noexcept { return m_pLowerRegion; }

	void MakeLambda() noexcept { m_bIsLambda = true; }
	[[nodiscard]] bool IsLambda() const noexcept { return m_bIsLambda; }

	std::unique_ptr<CVariableManager> m_VariableManager;
	std::unique_ptr<CFunctionManager> m_FunctionManager;

protected:

	CFileRuntimeData* const m_pFile{};
	CProgramContext* const m_pContext{};
	CMemory* m_pGlobal{ nullptr };
	CMemory* m_pLowerRegion{ nullptr };

private:
	VectorOf<RuntimeBlock> m_oInstructions;
	bool m_bIsLambda{ false };
};

using RuntimeBlock = std::unique_ptr<IRuntimeStructure>;

class CStack final : public CMemory
{
	NONCOPYABLE(CStack);
	friend class CFunctionLinter;
	friend class CIdentifierLinter;

public:
	CStack(CMemory* globalMemory, CFileRuntimeData* const file, CProgramContext* const context);
	CStack(CMemory* globalMemory, std::unique_ptr<CFunctionBlock>&& func, CFileRuntimeData* const file, CProgramContext* const context);
	~CStack();

	[[nodiscard]] bool IsStack() const noexcept override { return true; }
	[[nodiscard]] CStack* GetGlobalFunction();

	void AddFunctionInstruction(RuntimeBlock&& block) const;

	void AddSharedOwnershipVariable(std::size_t varIndex) { m_oIndicesWhichRequireSharedOwnership.insert(varIndex); }
	void AddArgumentVariable(std::size_t varIndex) { m_oArgumentIndices.insert(varIndex); }

	std::unique_ptr<CFunctionBlock> m_pFunction;
	CStack* m_pLowerFunction{ nullptr };

private:
	std::set<std::size_t> m_oIndicesWhichRequireSharedOwnership;
	std::set<std::size_t> m_oArgumentIndices;
};
