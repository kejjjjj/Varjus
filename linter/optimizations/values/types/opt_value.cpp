#ifdef OPTIMIZATIONS

#include "opt_value.hpp"
#include "linter/optimizations/optimizations.hpp"
#include "linter/expressions/ast/ast_main.hpp"
#include "opt_string.hpp"

void IConstEvalValue::ReleaseInternal()
{
	SetOwner(nullptr);
}
void IConstEvalValue::Release()
{
	ReleaseInternal();
	return COptimizationValues::FreeValue<IConstEvalValue>(this);
}
IConstEvalValue* IConstEvalValue::Copy()
{
	return COptimizationValues::AcquireNewValue<IConstEvalValue>();
}
std::shared_ptr<ConstantASTNode> IConstEvalValue::ToAST() const
{
	return std::make_shared<ConstantASTNode>(m_oApproximatePosition, "", t_undefined);
}

bool& IConstEvalValue::AsBoolean() {
	return ToCBoolean()->Get();
}
VarjusInt& IConstEvalValue::AsInt() {
	return ToCInt()->Get();
}
VarjusDouble& IConstEvalValue::AsDouble() {
	return ToCDouble()->Get();
}
std::string& IConstEvalValue::AsString() {
	return ToCString()->Get();
}

#endif