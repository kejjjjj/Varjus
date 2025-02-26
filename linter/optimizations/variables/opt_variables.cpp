#ifdef OPTIMIZATIONS

#include "opt_variables.hpp"
#include "linter/optimizations/optimizations.hpp"
#include "varjus_api/internal/runtime.hpp"

CConstEvalVariable::CConstEvalVariable() {}
CConstEvalVariable::~CConstEvalVariable() = default;

void CConstEvalVariable::SetValue(IConstEvalValue* v)
{
	assert(v);
	assert(m_pValue != v);

	// give the value back to the pool
	if (m_pValue) {
		assert(m_pValue->HasOwner());
		m_pValue->SetOwner(nullptr);
		m_pValue->Release();
	}

	m_pValue = v;
	m_pValue->SetOwner(this);
}

bool CConstEvalVariable::Release()
{
	assert(m_uRefCount > 0);
	if (m_uRefCount-- != 1) {
		return false;
	}

	auto& v = GetValue();
	assert(v && v->HasOwner());
	v->Release();
	v = nullptr;
	COptimizationValues::FreeVariable(this);
	return true;
}
CConstEvalVariable* COptimizationValues::AcquireNewVariable() {
	auto var = GetPool<CConstEvalVariable>().Acquire();
	var->RefCount() = 1;
	return var;
}
VectorOf<CConstEvalVariable*> COptimizationValues::AcquireNewVariables(std::size_t count) {

	return GetPool<CConstEvalVariable>().Acquire(count);
}
void COptimizationValues::FreeVariable(CConstEvalVariable* var) {
	assert(var->RefCount() == 0);
	var->RefCount() = 1;
	GetPool<CConstEvalVariable>().Release(var);
}

#endif