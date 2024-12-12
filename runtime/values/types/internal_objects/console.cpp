#include "console.hpp"
#include "runtime/runtime.hpp"
#include <iostream>

CConsoleValue::ConsoleMethods CConsoleValue::ConstructMethods()
{
	return {
		{"log", {UNCHECKED_PARAMETER_COUNT, &CConsoleValue::Log}},
	};
};

CConsoleValue* CConsoleValue::Construct()
{
	auto ptr = CProgramRuntime::AcquireNewValue<CConsoleValue>();
	ptr->MakeShared();
	return ptr;
}

void CConsoleValue::Release() {

	if (SharedRefCount() == 1) {
		m_pMethod = nullptr;
		Get().Release();
	}

	ReleaseInternal();
	CProgramRuntime::FreeValue<CConsoleValue>(this);
	ReleaseShared();
}

CConsoleValue* CConsoleValue::Copy() {
	CConsoleValue* ptr = CProgramRuntime::AcquireNewValue<CConsoleValue>();
	ptr->MakeShared();
	ptr->GetShared() = GetShared();
	ptr->m_pMethod = m_pMethod;

	return ptr;
}

IValue* CConsoleValue::GetAggregate(std::size_t memberIdx) {

	if (auto func = CBuiltInMethods<CConsoleValue>::LookupMethod(memberIdx)) {
		auto ptr = HasOwner() ? this : this->Copy();
		ptr->m_pMethod = func;
		return ptr;
	}

	return CObjectValue::GetAggregate(memberIdx);
}

IValue* CConsoleValue::Call(CFunction* const thisFunction, const IValues& args)
{
	assert(IsCallable());
	auto ret = CBuiltInMethods<CConsoleValue>::CallMethod(thisFunction, this, args, m_pMethod);
	m_pMethod = nullptr;
	return ret;
}

IValue* CConsoleValue::Log([[maybe_unused]] CFunction* const thisFunction, const IValues& values)
{

	std::string p;

	for (auto& v : values)
		p += v->ValueAsString();

	std::cout << p << '\n';

	return CProgramRuntime::AcquireNewValue<IValue>();
}

