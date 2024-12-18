#include "console.hpp"
#include "runtime/runtime.hpp"
#include <iostream>

CConsoleValue::ConsoleMethods CConsoleValue::ConstructMethods()
{
	return {
		{"log",   {UNCHECKED_PARAMETER_COUNT, &CConsoleValue::Log}},
		/*{"clear", {0u, &CConsoleValue::Clear}},*/
	};
};

CConsoleValue* CConsoleValue::Construct()
{
	auto ptr = CProgramRuntime::AcquireNewValue<CConsoleValue>();
	ptr->MakeShared();
	ptr->m_pMethod = nullptr;
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

IValue* CConsoleValue::Call(CRuntimeContext* const ctx, const IValues& args)
{
	if(!IsCallable())
		throw CRuntimeError(std::format("a value of type \"{}\" is not callable", TypeAsString()));

	auto ret = CBuiltInMethods<CConsoleValue>::CallMethod(ctx, this, args, m_pMethod);
	m_pMethod = nullptr;
	return ret;
}

IValue* CConsoleValue::Log([[maybe_unused]] CRuntimeContext* const ctx, const IValues& values)
{

	std::string p;

	for (auto& v : values)
		p += v->ValueAsString();

	std::cout << p << '\n';

	return CProgramRuntime::AcquireNewValue<IValue>();
}

//IValue* CConsoleValue::Clear([[maybe_unused]] CRuntimeContext* const ctx, [[maybe_unused]] const IValues& values)
//{
//#if _WIN32
//	system("cls");
//#else
//	#error "Unsupported OS"
//#endif
//	return CProgramRuntime::AcquireNewValue<IValue>();
//}