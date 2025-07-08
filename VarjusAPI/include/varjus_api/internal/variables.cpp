#include "variables.hpp"
#include "varjus_api/types/types.hpp"
#include "varjus_api/internal/runtime.hpp"

CVariable* CVariable::Construct(Varjus::CProgramRuntime* const runtime)
{
	auto v = runtime->AcquireNewVariable();
	v->m_pAllocator = runtime;
	return v;
}
CVariable* CVariable::Construct(Varjus::CProgramRuntime* const runtime, IValue* v)
{
	auto var = runtime->AcquireNewVariable();
	var->SetValue(v);
	return var;
}

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
	if (m_uRefCount != 1) {
		m_uRefCount--;
		return false;
	}

	m_uRefCount = 0;

	auto& v = GetValue();
	assert(v && v->HasOwner());
	v->Release();
	v = nullptr;
	m_bSelfCapturing = false;
	m_pAllocator->FreeVariable(this);	
	return true;
}

using namespace Varjus;

CVariable* CProgramRuntime::AcquireNewVariable(){
	auto var = GetPool<CVariable>().Acquire();
	var->RefCount() = 1;
	var->m_pAllocator = this;
	return var;
}
VectorOf<CVariable*> CProgramRuntime::AcquireNewVariables(std::size_t count){
	auto&& vars = GetPool<CVariable>().Acquire(count);

	for (auto& v : vars)
		v->m_pAllocator = this;

	return vars;
}
void CProgramRuntime::FreeVariable(CVariable* var){
	assert(var->RefCount() == 0);
	var->RefCount() = 1;
	GetPool<CVariable>().Release(var);
	var->m_pAllocator = nullptr;
}

CChildVariable* CProgramRuntime::AcquireNewChildVariable(const ArrayOwner& parent)
{
	auto var = GetPool<CChildVariable>().Acquire();
	var->RefCount() = 1;
	var->m_pAllocator = this;
	var->SetParent(parent);
	return var;
}
VectorOf<CChildVariable*> CProgramRuntime::AcquireNewChildVariables(std::size_t count, const ArrayOwner& parent)
{
	auto&& vars = GetPool<CChildVariable>().Acquire(count);

	for (auto& v : vars) {
		v->m_pAllocator = this;
		v->SetParent(parent);
	}
	return vars;
}
void CProgramRuntime::FreeChildVariable(CChildVariable* var)
{
	assert(var->RefCount() == 0);
	var->RefCount() = 1;
	GetPool<CChildVariable>().Release(var);
	var->m_pAllocator = nullptr;
	var->SetParent({});
}

CChildVariable* CChildVariable::Construct(Varjus::CProgramRuntime* const runtime, const ArrayOwner& parent) {
	auto v = runtime->AcquireNewChildVariable(parent);
	v->m_pAllocator = runtime;
	return v;
}
CChildVariable* CChildVariable::Construct(Varjus::CProgramRuntime* const runtime, IValue* v, const ArrayOwner& parent) {
	auto var = runtime->AcquireNewChildVariable(parent);
	var->SetValue(v);
	return var;
}

bool CChildVariable::Release()
{
	assert(m_uRefCount > 0);
	if (m_uRefCount != 1) {
		m_uRefCount--;
		return false;
	}

	m_uRefCount = 0;

	auto& v = GetValue();
	assert(v && v->HasOwner());
	v->Release();
	v = nullptr;
	m_bSelfCapturing = false;
	m_pAllocator->FreeChildVariable(this);
	return true;
}