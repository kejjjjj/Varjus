#include "varjus_api/internal/structure.hpp"
#include "varjus_api/internal/runtime.hpp"

#include "linter/expressions/ast.hpp"

CRuntimeThrowStatement::CRuntimeThrowStatement(ASTNode&& condition)
	: m_pAST(std::move(condition)) {
}
CRuntimeThrowStatement::~CRuntimeThrowStatement() = default;;

IValue* CRuntimeThrowStatement::Execute(Varjus::CRuntimeContext* const ctx)
{
	assert(m_pAST.get());
	auto v = CRuntimeExpression::Evaluate(ctx, m_pAST);

	auto ret = v->HasOwner() ? v->Copy() : v;

	ctx->m_pRuntime->GetExceptionValue() = ret;
	ctx->m_pRuntime->ThrowException();

	return ret;
}