#include "runtime/structure.hpp"
#include "runtime/runtime.hpp"

#include "linter/expressions/ast.hpp"

CRuntimeReturnStatement::CRuntimeReturnStatement(ASTNode&& condition) 
	: m_pAST(std::move(condition)){}
CRuntimeReturnStatement::~CRuntimeReturnStatement() = default;;

IValue* CRuntimeReturnStatement::Execute(CRuntimeContext* const ctx) 
{
	if (!m_pAST) // no return expression, return undefined
		return CProgramRuntime::AcquireNewValue<IValue>();

	return CRuntimeExpression::Evaluate(ctx, m_pAST.get());
}