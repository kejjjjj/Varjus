#pragma once

#include <vector>
#include <unordered_map>
#include <string>

class CVariableDeclaration;

class CMemoryData
{
public:
	CMemoryData();
	virtual ~CMemoryData();

	void DeclareVariable(const std::string& var);
	[[nodiscard]] bool ContainsVariable(const std::string_view& name) const;

	[[nodiscard]] virtual bool IsStack() const noexcept { return false; }

protected:
	std::vector<std::string> m_oVariables;
};

class CStack : public CMemoryData
{
public:
	CStack();
	~CStack();

private:

	[[nodiscard]] bool IsStack() const noexcept override { return true; }
};
