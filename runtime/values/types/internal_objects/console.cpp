#include "console.hpp"
#include "runtime/runtime.hpp"
#include "runtime/structure.hpp"

#include "linter/context.hpp"
#include <iostream>

DECLARE_BUILT_IN_METHODS CConsoleValue::m_oMethods;

FORWARD_DECLARE_METHOD(LogConsole);

void CConsoleValue::ConstructMethods()
{
	m_oMethods.clear();

	ADD_METHOD("log", LogConsole, UNCHECKED_PARAMETER_COUNT);
}

CConsoleValue* CConsoleValue::Construct()
{
	auto ptr = CProgramRuntime::AcquireNewValue<CConsoleValue>();
	ptr->MakeShared();
	return ptr;
}

void CConsoleValue::Release() {

	if (SharedRefCount() == 1) {
		Get().Release();
	}

	ReleaseInternal();
	CProgramRuntime::FreeValue<CConsoleValue>(this);
	ReleaseShared();
}

IValue* CConsoleValue::Copy() {

	CConsoleValue* ptr = CProgramRuntime::AcquireNewValue<CConsoleValue>();
	ptr->MakeShared();
	ptr->GetShared() = GetShared();

	return ptr;
}

IValue* CConsoleValue::GetAggregate(std::size_t memberIdx) {

	if (m_oMethods.contains(memberIdx)) {
		auto v = CProgramRuntime::AcquireNewValue<CCallableValue>();
		METHOD_BIND(v, this->Copy());
		return v;
	}

	return CObjectValue::GetAggregate(memberIdx);
}

DEFINE_METHOD(LogConsole)
{
	std::string p;

	for (auto& v : newValues)
		p += v->ValueAsString();

	std::cout << p << '\n';

	return CProgramRuntime::AcquireNewValue<IValue>();
}