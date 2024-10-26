#pragma once
#include <vector>
#include <memory>
#include <string>
#include "globalDefinitions.hpp"

/***********************************************************************
 > represents the runtime execution structure
***********************************************************************/
class AbstractSyntaxTree;

enum EStructureType
{
	st_function,
	st_expression
};

class CRuntimeFunction;
struct CFunctionBlock;

class CRuntimeStructure
{
	NONCOPYABLE(CRuntimeStructure);
public:
	CRuntimeStructure();
	virtual ~CRuntimeStructure();
	[[nodiscard]] virtual constexpr EStructureType Type() const noexcept = 0;

protected:
	CRuntimeFunction* m_pOwner{nullptr};
};

class CRuntimeFunction final : public CRuntimeStructure
{
	NONCOPYABLE(CRuntimeFunction);

public:
	CRuntimeFunction(CFunctionBlock& linterFunction);
	~CRuntimeFunction();

	auto& GetName() const noexcept { return m_sName; }

protected:
	[[nodiscard]] constexpr EStructureType Type() const noexcept { return st_function; };

	std::string m_sName;
	std::size_t uNumParameters{ 0u };
	std::vector<std::unique_ptr<CRuntimeStructure>> m_oInstructions;
};


class CRuntimeExpression final : public CRuntimeStructure
{
	NONCOPYABLE(CRuntimeExpression);
public:
	CRuntimeExpression(std::unique_ptr<AbstractSyntaxTree>&& ast);
	~CRuntimeExpression();

	std::unique_ptr<AbstractSyntaxTree> m_pAST;
protected:
	[[nodiscard]] constexpr EStructureType Type() const noexcept { return st_expression;};
};

using RuntimeBlock = std::unique_ptr<CRuntimeStructure>;
using RuntimeFunction = std::unique_ptr<CRuntimeFunction>;

class IRuntimeBlock
{
public:
	virtual ~IRuntimeBlock() = default;

protected:
	virtual RuntimeBlock ToRuntimeObject() const = 0;
};

class CFileRuntimeData final
{
	friend class CProgramRuntime;
	NONCOPYABLE(CFileRuntimeData)
public:
	CFileRuntimeData() = default;
	constexpr void AddFunction(RuntimeFunction&& func) { m_oFunctions.emplace_back(std::move(func)); }

private:
	std::vector<RuntimeFunction> m_oFunctions;
};
