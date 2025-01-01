#include "opt_integer.hpp"
#include "linter/optimizations/optimizations.hpp"
#include "linter/expressions/ast/ast_main.hpp"
void CConstEvalIntValue::Release()
{
	ReleaseInternal();
	COptimizationValues::FreeValue<CConstEvalIntValue>(this);
}
IConstEvalValue* CConstEvalIntValue::Copy()
{
	return COptimizationValues::AcquireNewValue<CConstEvalIntValue>(Get());
}
std::shared_ptr<ConstantASTNode> CConstEvalIntValue::ToAST() const
{
	std::string data;
	data.append(reinterpret_cast<const char*>(&Get()), sizeof(Get()));
	return std::make_shared<ConstantASTNode>(m_oApproximatePosition, data, t_int);
}