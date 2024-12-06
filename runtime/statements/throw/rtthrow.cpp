#include "runtime/structure.hpp"
#include "runtime/runtime.hpp"

#include "linter/expressions/ast.hpp"

CRuntimeThrowStatement::CRuntimeThrowStatement(std::unique_ptr<AbstractSyntaxTree>&& condition)
	: m_pAST(std::move(condition)) {
}
CRuntimeThrowStatement::~CRuntimeThrowStatement() = default;;

IValue* CRuntimeThrowStatement::Execute(CFunction* const thisFunction)
{
	assert(m_pAST.get());
	CProgramRuntime::ThrowException();
	return CRuntimeExpression::Evaluate(thisFunction, m_pAST.get());
}