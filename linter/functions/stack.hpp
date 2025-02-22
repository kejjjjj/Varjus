#pragma once

#include <vector>
#include <memory>
#include <set>

#include "api/internal/globalDefinitions.hpp"

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

template<typename T>
using VarManager = std::unique_ptr<CVariableManager<T>>;

class CMemory
{
	NONCOPYABLE(CMemory);

	friend class CFunctionLinter;
	friend class CIdentifierLinter;
	friend class CFunctionManager;
	friend class CVariableDeclarationLinter;
	friend class CBufferLinter;

public:
	CMemory(CMemory* globalMemory, CModule* const file);
	virtual ~CMemory();

	[[nodiscard]] virtual bool IsStack() const noexcept { return false; }

	[[nodiscard]] CStack* ToStack();
	[[nodiscard]] auto ToStack() const;
	[[nodiscard]] virtual constexpr bool IsLocalFunction() const noexcept { return false; };

	[[nodiscard]] constexpr auto& GetModule() noexcept { return m_pGlobal->m_pModule; }

	[[nodiscard]] CFileContext* GetContext() const;
	[[nodiscard]] auto& GetGlobalMemory() noexcept { return m_pGlobal; }
	[[nodiscard]] auto& GetGlobalMemory() const noexcept { return m_pGlobal; }

	[[nodiscard]] bool IsGlobalMemory() const noexcept { return this == m_pGlobal; }
	[[nodiscard]] auto& GetLowerMemoryRegion() const noexcept { return m_pLowerRegion; }


	[[nodiscard]] bool IsHoisting() const noexcept { return !m_pGlobal->m_pHoister; }
	[[nodiscard]] bool HasHoistedData() const noexcept { return !!m_pGlobal->m_pHoister; }
	[[nodiscard]] auto& GetHoister() const noexcept { return m_pGlobal->m_pHoister; }

	VarManager<CLinterVariable> m_VariableManager;
#ifdef OPTIMIZATIONS
	VarManager<CConstEvalLinterVariable> m_ConstEvalVariableManager;
#endif
	std::unique_ptr<CFunctionManager> m_FunctionManager;

protected:

	CModule* const m_pModule{};
	CMemory* m_pGlobal{ nullptr };
	CMemory* m_pLowerRegion{ nullptr };

private:
	VectorOf<RuntimeBlock> m_oInstructions;
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
	[[nodiscard]] constexpr bool IsLocalFunction() const noexcept override { return !!m_pLowerFunction; };

	void AddFunctionInstruction(RuntimeBlock&& block) const;

	std::unique_ptr<CFunctionBlock> m_pFunction;
	CStack* m_pLowerFunction{ nullptr };

private:
};
