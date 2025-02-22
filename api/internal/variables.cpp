#include "variables.hpp"
#include "api/types/types.hpp"
#include "api/internal/runtime.hpp"

CVariable* CVariable::Construct()
{
	return CProgramRuntime::AcquireNewVariable();
}
CVariable* CVariable::Construct(IValue* v)
{
	auto var = CProgramRuntime::AcquireNewVariable();
	var->SetValue(v);
	return var;
}

CVariable::CVariable() {}
CVariable::~CVariable() = default;

void CVariable::SetValue(IValue* v)
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

bool CVariable::Release()
{
	assert(m_uRefCount > 0);
	if (m_uRefCount-- != 1) {
		return false;
	}

	auto& v = GetValue();
	assert(v && v->HasOwner());
	v->Release();
	v = nullptr;
	m_bSelfCapturing = false;
	CProgramRuntime::FreeVariable(this);	
	return true;
}


CVariable* CProgramRuntime::AcquireNewVariable(){
	auto var = GetPool<CVariable>().Acquire();
	var->RefCount() = 1;
	return var;
}
VectorOf<CVariable*> CProgramRuntime::AcquireNewVariables(std::size_t count){

	return GetPool<CVariable>().Acquire(count);
}
void CProgramRuntime::FreeVariable(CVariable* var){
	assert(var->RefCount() == 0);
	var->RefCount() = 1;
	GetPool<CVariable>().Release(var);
}
