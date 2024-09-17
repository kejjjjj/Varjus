#pragma once

#include <vector>
#include <unordered_map>
#include <string>

class CVariableDeclaration;

struct CLinterVariable
{
	CLinterVariable() = default;
	CLinterVariable(const std::string& name, std::size_t index) : m_sName(name), m_uIndex(index) {}

	//CLinterVariable operator=(const CLinterVariable&) = delete;

	std::string m_sName;
	std::size_t m_uIndex{};

};

class CMemoryData
{
public:
	CMemoryData();
	virtual ~CMemoryData();

	CLinterVariable* DeclareVariable(const std::string& var);
	CLinterVariable* GetVariable(const std::string& var);

	[[nodiscard]] bool ContainsVariable(const std::string& name) const;

	[[nodiscard]] virtual bool IsStack() const noexcept { return false; }

protected:
	std::unordered_map<std::string, CLinterVariable> m_oVariables;
};

class CStack : public CMemoryData
{
public:
	CStack();
	~CStack();

private:

	[[nodiscard]] bool IsStack() const noexcept override { return true; }
};
