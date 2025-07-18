#include "varjus_api/internal/structure.hpp"
#include "varjus_api/internal/runtime.hpp"

#include "linter/expressions/ast.hpp"

CRuntimeReturnStatement::CRuntimeReturnStatement(ASTNode&& condition) 
	: m_pAST(std::move(condition)){}
CRuntimeReturnStatement::~CRuntimeReturnStatement() = default;;

IValue* CRuntimeReturnStatement::Execute(Varjus::CRuntimeContext* const ctx) 
{
	if (!m_pAST) // no return expression, return undefined
		return IValue::Construct(ctx->m_pRuntime);

	return CRuntimeExpression::Evaluate(ctx, m_pAST);
}