#include "variables.hpp"
#include "values/types/types.hpp"
#include "runtime/runtime.hpp"
#include <iostream>



CVariable::CVariable() {}
CVariable::~CVariable() = default;

void CVariable::SetValue(IValue* v)
{
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

void CVariable::Release()
{
	auto& v = GetValue();
	assert(v->HasOwner());
	v->Release();
	v = nullptr;
	CProgramRuntime::FreeVariable(this); 
}


CVariable* CProgramRuntime::AcquireNewVariable(){
	return GetPool<CVariable>().Acquire();
}
VectorOf<CVariable*> CProgramRuntime::AcquireNewVariables(std::size_t count){
	return GetPool<CVariable>().Acquire(count);
}
void CProgramRuntime::FreeVariable(CVariable* var)
{
	GetPool<CVariable>().Release(var);
}
