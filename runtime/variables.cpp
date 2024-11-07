#include "variables.hpp"
#include "values/types/types.hpp"
#include "runtime/runtime.hpp"
#include <iostream>


CVariable::CVariable() {}
CVariable::~CVariable() = default;


void CVariable::SetValue(IValue* v)
{
	bool hadvalue = false;
	// give the value back to the pool
	if (m_pValue && m_pValue->HasOwner()) {
		m_pValue->SetOwner(nullptr);
		m_pValue->Release();
		hadvalue = true;
	}

	m_pValue = v;
	m_pValue->SetOwner(this);

}