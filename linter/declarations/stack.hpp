#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <memory>

#include "globalDefinitions.hpp"

class CVariableDeclaration;
class CMemory;
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
public:
	CMemory();
	virtual ~CMemory();

	[[maybe_unused]] CLinterVariable* DeclareVariable(const std::string& var);
	[[nodiscard]] CLinterVariable* GetVariable(const std::string& var);

	[[nodiscard]] bool ContainsVariable(const std::string& name) const;

	[[nodiscard]] virtual bool IsStack() const noexcept { return false; }

protected:
	std::unordered_map<std::string, CLinterVariable> m_oVariables;
};

class CStack final : public CMemory
{
	NONCOPYABLE(CStack);
public:
	CStack(std::unique_ptr<CFunctionBlock>&& func);
	~CStack();

private:

	[[nodiscard]] bool IsStack() const noexcept override { return true; }
	std::unique_ptr<CFunctionBlock> m_pFunction;
};
