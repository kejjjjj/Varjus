#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <memory>

#include "globalDefinitions.hpp"

class CVariableDeclarationLinter;
class CMemory;
class CStack;
class IRuntimeStructure;
class CFileRuntimeData;

struct CFunctionBlock;

struct CLinterVariable final
{
	CLinterVariable() = default;
	CLinterVariable(const CMemory* owner, const std::string& name, std::size_t index) : m_pOwner(owner), m_sName(name), m_uIndex(index) {}

	const CMemory* m_pOwner{};
	std::string m_sName;
	std::size_t m_uIndex{};
};

class CMemory
{
	friend class CFunctionLinter;
public:
	CMemory(CFileRuntimeData* const file);
	virtual ~CMemory();

	[[nodiscard]] virtual bool IsStack() const noexcept { return false; }

	[[maybe_unused]] CLinterVariable* DeclareVariable(const std::string& var);
	[[nodiscard]] CLinterVariable* GetVariable(const std::string& var);
	[[nodiscard]] bool ContainsVariable(const std::string& name) const;
	[[nodiscard]] std::size_t GetVariableCount() const noexcept;

	[[nodiscard]] CStack* ToStack();
	[[nodiscard]] auto ToStack() const;
protected:
	std::unordered_map<std::string, CLinterVariable> m_oVariables;
	CFileRuntimeData* const m_pFile{};
};

using RuntimeBlock = std::unique_ptr<IRuntimeStructure>;

class CStack final : public CMemory
{
	NONCOPYABLE(CStack);
public:
	CStack(CFileRuntimeData* const file);
	CStack(std::unique_ptr<CFunctionBlock>&& func, CFileRuntimeData* const file);
	~CStack();

	void AddFunctionInstruction(RuntimeBlock&& block) const;

	[[nodiscard]] bool IsStack() const noexcept override { return true; }
	std::unique_ptr<CFunctionBlock> m_pFunction;
};
