#pragma once
#include <memory>

/***********************************************************************
 > represents the runtime execution structure
***********************************************************************/

class AbstractSyntaxTree;

enum EStructureType
{
	st_expression
};

class CCodeStructure
{
public:
	[[nodiscard]] virtual constexpr EStructureType Type() const noexcept = 0;

};
class CCodeExpression final : public CCodeStructure
{
public:
	std::unique_ptr<AbstractSyntaxTree> m_pAST;
protected:
	[[nodiscard]] constexpr EStructureType Type() const noexcept { return st_expression;};
};
