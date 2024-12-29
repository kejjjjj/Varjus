#include "opt_value.hpp"
#include "linter/optimizations/optimizations.hpp"

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