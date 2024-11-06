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

enum EMemoryIdentifierType
{
	mi_variable,
	mi_function
};

struct CMemoryIdentifier
{
	CMemoryIdentifier() = default;
	CMemoryIdentifier(const std::string& name, std::size_t index) 
		: m_sName(name), m_uIndex(index) {}
	virtual ~CMemoryIdentifier() = default;

	[[nodiscard]] virtual constexpr EMemoryIdentifierType Type() const noexcept = 0;

	std::string m_sName;
	std::size_t m_uIndex{};
};

struct CLinterVariable final : public CMemoryIdentifier
{
	CLinterVariable() = default;
	CLinterVariable(const CMemory* owner, const std::string& name, std::size_t index) 
		: CMemoryIdentifier(name, index), m_pOwner(owner) {}

	[[nodiscard]] virtual constexpr EMemoryIdentifierType Type() const noexcept override { return mi_variable; }

	const CMemory* m_pOwner{};

};
struct CLinterFunction final : public CMemoryIdentifier
{
	CLinterFunction() = default;
	CLinterFunction(const std::string& name, std::size_t index) 
		: CMemoryIdentifier(name, index) {}

	[[nodiscard]] virtual constexpr EMemoryIdentifierType Type() const noexcept override { return mi_function; }
};
class CMemory
{
	friend class CFunctionLinter;
	friend class CIdentifierLinter;
public:
	CMemory(CFileRuntimeData* const file);
	virtual ~CMemory();

	[[nodiscard]] virtual bool IsStack() const noexcept { return false; }

	[[maybe_unused]] CLinterVariable* DeclareVariable(const std::string& var);
	[[nodiscard]] CLinterVariable* GetVariable(const std::string& var);
	[[nodiscard]] bool ContainsVariable(const std::string& name) const;
	[[nodiscard]] std::size_t GetVariableCount() const noexcept;

	[[maybe_unused]] CLinterFunction* DeclareFunction(const std::string& var);
	[[nodiscard]] CLinterFunction* GetFunction(const std::string& var);
	[[nodiscard]] std::size_t GetFunctionIndex(const std::string& var);
	[[nodiscard]] bool ContainsFunction(const std::string& name) const;
	[[nodiscard]] bool ContainsFunctionGlobally(const std::string& name) const;

	[[nodiscard]] std::size_t GetFunctionCount() const noexcept;

	[[nodiscard]] CStack* ToStack();
	[[nodiscard]] auto ToStack() const;
protected:

	std::unordered_map<std::string, CLinterVariable> m_oVariables;
	std::unordered_map<std::string, CLinterFunction> m_oFunctions;

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
