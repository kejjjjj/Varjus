
#ifdef OPTIMIZATIONS

#include "opt_boolean.hpp"
#include "linter/optimizations/optimizations.hpp"
#include "linter/expressions/ast/ast_main.hpp"

void CConstEvalBooleanValue::Release()
{
	ReleaseInternal();
	COptimizationValues::FreeValue<CConstEvalBooleanValue>(this);
}
IConstEvalValue* CConstEvalBooleanValue::Copy()
{
	return COptimizationValues::AcquireNewValue<CConstEvalBooleanValue>(Get());
}
std::shared_ptr<ConstantASTNode> CConstEvalBooleanValue::ToAST() const
{
	VarjusString data;
	data.append(reinterpret_cast<const char*>(&Get()), sizeof(Get()));
	return std::make_shared<ConstantASTNode>(m_oApproximatePosition, data, t_boolean);
}

#endif