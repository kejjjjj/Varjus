#pragma once

#include <vector>
#include <memory>
#include <set>

#include "globalDefinitions.hpp"

#include "memory/manager_variable.hpp"
#include "memory/manager_function.hpp"

class CStack;
class IRuntimeStructure;
class CModule;
class CHoister;

struct CFunctionBlock;
struct CFileContext;

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
	friend class CFileLinter;

public:
	CMemory(CMemory* globalMemory, CModule* const file);
	virtual ~CMemory();

	[[nodiscard]] virtual bool IsStack() const noexcept { return false; }

	[[nodiscard]] CStack* ToStack();
	[[nodiscard]] auto ToStack() const;

	[[nodiscard]] constexpr auto& GetModule() noexcept { return m_pGlobal->m_pModule; }

	[[nodiscard]] CFileContext* GetContext() const;
	[[nodiscard]] auto& GetGlobalMemory() noexcept { return m_pGlobal; }
	[[nodiscard]] auto& GetGlobalMemory() const noexcept { return m_pGlobal; }

	[[nodiscard]] bool IsGlobalMemory() const noexcept { return this == m_pGlobal; }
	[[nodiscard]] auto& GetLowerMemoryRegion() const noexcept { return m_pLowerRegion; }


	[[nodiscard]] bool IsHoisting() const noexcept { return !m_pGlobal->m_pHoister; }
	[[nodiscard]] bool HasHoistedData() const noexcept { return !!m_pGlobal->m_pHoister; }
	[[nodiscard]] auto& GetHoister() const noexcept { return m_pGlobal->m_pHoister; }

	void MakeLambda() noexcept { m_bIsLambda = true; }
	[[nodiscard]] bool IsLambda() const noexcept { return m_bIsLambda; }

	std::unique_ptr<CVariableManager> m_VariableManager;
	std::unique_ptr<CFunctionManager> m_FunctionManager;

protected:

	CModule* const m_pModule{};
	CMemory* m_pGlobal{ nullptr };
	CMemory* m_pLowerRegion{ nullptr };

private:
	VectorOf<RuntimeBlock> m_oInstructions;
	bool m_bIsLambda{ false };
	CHoister* m_pHoister{ nullptr };
};

using RuntimeBlock = std::unique_ptr<IRuntimeStructure>;

class CStack final : public CMemory
{
	NONCOPYABLE(CStack);
	friend class CFunctionLinter;
	friend class CIdentifierLinter;

public:
	CStack(CMemory* globalMemory, CModule* const file);
	CStack(CMemory* globalMemory, std::unique_ptr<CFunctionBlock>&& func, CModule* const file);
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
