#include "opt_integer.hpp"
#include "linter/optimizations/optimizations.hpp"

void CConstEvalIntValue::Release()
{
	ReleaseInternal();
	COptimizationValues::FreeValue<CConstEvalIntValue>(this);
}
IConstEvalValue* CConstEvalIntValue::Copy()
{
	return COptimizationValues::AcquireNewValue<CConstEvalIntValue>(Get());
}