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
	return ptr;
}

void CConsoleValue::Release() {

	if (SharedRefCount() == 1) {
		Get().Release();
	}

	ReleaseInternal();
	CDataTypeMethods::Release();
	CProgramRuntime::FreeValue<CConsoleValue>(this);
	ReleaseShared();
}

IValue* CConsoleValue::Copy() {

	if (auto c = CDataTypeMethods::Copy())
		return c;

	CConsoleValue* ptr = CProgramRuntime::AcquireNewValue<CConsoleValue>();
	ptr->MakeShared();
	ptr->GetShared() = GetShared();

	return ptr;
}

IValue* CConsoleValue::GetAggregate(std::size_t memberIdx) {

	if (auto func = CDataTypeMethods::FindMethod(memberIdx)) {
		auto newValue = HasOwner() ? this : static_cast<CConsoleValue*>(this->Copy());
		newValue->SetMethod(func);
		return newValue;
	}

	return CObjectValue::GetAggregate(memberIdx);
}

IValue* CConsoleValue::Call(CRuntimeContext* const ctx, const IValues& args)
{
	if(!IsCallable())
		throw CRuntimeError(std::format("a value of type \"{}\" is not callable", TypeAsString()));

	auto ret = CBuiltInMethods<CConsoleValue>::CallMethod(ctx, this, args, GetMethod());
	CDataTypeMethods::Release();
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