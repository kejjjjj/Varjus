#include "runtime/structure.hpp"
#include "runtime/runtime.hpp"

#include "linter/expressions/ast.hpp"

CRuntimeReturnStatement::CRuntimeReturnStatement(std::unique_ptr<AbstractSyntaxTree>&& condition) 
	: m_pAST(std::move(condition)){}
CRuntimeReturnStatement::~CRuntimeReturnStatement() = default;;

IValue* CRuntimeReturnStatement::Execute(CFunction* const thisFunction) 
{
	if (!m_pAST) // no return expression, return undefined
		return CProgramRuntime::AcquireNewValue<IValue>();

	return CRuntimeExpression::Evaluate(thisFunction, m_pAST.get());
}