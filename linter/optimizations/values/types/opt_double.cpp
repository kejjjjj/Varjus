#include "opt_double.hpp"
#include "linter/optimizations/optimizations.hpp"
#include "linter/expressions/ast/ast_main.hpp"
void CConstEvalDoubleValue::Release()
{
	ReleaseInternal();
	COptimizationValues::FreeValue<CConstEvalDoubleValue>(this);
}
IConstEvalValue* CConstEvalDoubleValue::Copy()
{
	return COptimizationValues::AcquireNewValue<CConstEvalDoubleValue>(Get());
}
std::shared_ptr<ConstantASTNode> CConstEvalDoubleValue::ToAST() const
{
	std::string data;
	data.append(reinterpret_cast<const char*>(&Get()), sizeof(Get()));
	return std::make_shared<ConstantASTNode>(m_oApproximatePosition, data, t_double);
}