#include "opt_string.hpp"
#include "linter/optimizations/optimizations.hpp"
#include "linter/expressions/ast/ast_main.hpp"

void CConstEvalStringValue::Release()
{
	ReleaseInternal();
	COptimizationValues::FreeValue<CConstEvalStringValue>(this);
}
IConstEvalValue* CConstEvalStringValue::Copy()
{
	return COptimizationValues::AcquireNewValue<CConstEvalStringValue>(Get());
}
std::shared_ptr<ConstantASTNode> CConstEvalStringValue::ToAST() const
{
	return std::make_shared<ConstantASTNode>(m_oApproximatePosition, m_oValue, t_string);
}