#include "runtime/structure.hpp"
#include "runtime/runtime.hpp"

#include "linter/expressions/ast.hpp"

CRuntimeThrowStatement::CRuntimeThrowStatement(ASTNode&& condition)
	: m_pAST(std::move(condition)) {
}
CRuntimeThrowStatement::~CRuntimeThrowStatement() = default;;

IValue* CRuntimeThrowStatement::Execute(CRuntimeContext* const ctx)
{
	assert(m_pAST.get());
	auto v = CRuntimeExpression::Evaluate(ctx, m_pAST.get());

	auto ret = v->HasOwner() ? v->Copy() : v;

	CProgramRuntime::GetExceptionValue() = ret;
	CProgramRuntime::ThrowException();

	return ret;
}