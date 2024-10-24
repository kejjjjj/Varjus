#pragma once
#include <vector>
#include <memory>

#include "globalDefinitions.hpp"

/***********************************************************************
 > represents the runtime execution structure
***********************************************************************/
class AbstractSyntaxTree;

enum EStructureType
{
	st_expression
};

class CCodeFunction;

class CCodeStructure
{
	NONCOPYABLE(CCodeStructure);
public:
	CCodeStructure();
	virtual ~CCodeStructure();
	[[nodiscard]] virtual constexpr EStructureType Type() const noexcept = 0;

private:
	CCodeFunction* m_pOwner{nullptr};
};

class CCodeFunction final : public CCodeStructure
{
	NONCOPYABLE(CCodeFunction);
public:
protected:
	[[nodiscard]] constexpr EStructureType Type() const noexcept { return st_expression; };

	std::vector<std::unique_ptr<CCodeStructure>> m_oInstructions;
};
class CCodeExpression final : public CCodeStructure
{
	NONCOPYABLE(CCodeExpression);
public:
	CCodeExpression(std::unique_ptr<AbstractSyntaxTree>&& ast);
	~CCodeExpression();

	std::unique_ptr<AbstractSyntaxTree> m_pAST;
protected:
	[[nodiscard]] constexpr EStructureType Type() const noexcept { return st_expression;};
};

using RuntimeBlock = std::unique_ptr<CCodeStructure>;

class IRuntimeBlock
{
public:
	virtual ~IRuntimeBlock() = default;

protected:
	virtual RuntimeBlock ToRuntimeObject() const = 0;
};
