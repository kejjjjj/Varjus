#include "exceptions/exception.hpp"
#include "modules/rtmodule.hpp"
#include "runtime.hpp"
#include "structure.hpp"
#include "values/types/internal/builtin_methods.hpp"
#include "values/types/internal_objects/console.hpp"
#include "values/types/types.hpp"
#include "variables.hpp"

#include <chrono>
#include <iostream>
#include <ranges>

#define VALUEPOOL_INIT_SIZE size_t(100)

template<> COwningObjectPool<CVariable>           CProgramRuntime::m_oValuePool<CVariable>     (VALUEPOOL_INIT_SIZE);
template<> COwningObjectPool<IValue>              CProgramRuntime::m_oValuePool<IValue>        (VALUEPOOL_INIT_SIZE);
template<> COwningObjectPool<CBooleanValue>       CProgramRuntime::m_oValuePool<CBooleanValue> (VALUEPOOL_INIT_SIZE);
template<> COwningObjectPool<CIntValue>           CProgramRuntime::m_oValuePool<CIntValue>     (VALUEPOOL_INIT_SIZE);
template<> COwningObjectPool<CDoubleValue>        CProgramRuntime::m_oValuePool<CDoubleValue>  (VALUEPOOL_INIT_SIZE);
template<> COwningObjectPool<CStringValue>        CProgramRuntime::m_oValuePool<CStringValue>  (VALUEPOOL_INIT_SIZE);
template<> COwningObjectPool<CCallableValue>      CProgramRuntime::m_oValuePool<CCallableValue>(VALUEPOOL_INIT_SIZE);
template<> COwningObjectPool<CArrayValue>         CProgramRuntime::m_oValuePool<CArrayValue>   (VALUEPOOL_INIT_SIZE);
template<> COwningObjectPool<CObjectValue>        CProgramRuntime::m_oValuePool<CObjectValue>  (VALUEPOOL_INIT_SIZE);
template<> COwningObjectPool<CConsoleValue>       CProgramRuntime::m_oValuePool<CConsoleValue> (VALUEPOOL_INIT_SIZE);

RuntimeModules CProgramRuntime::m_oModules;
const CodePosition* CProgramRuntime::m_pCodePosition{ nullptr };
bool CProgramRuntime::m_bExceptionThrown{ false };
IValue* CProgramRuntime::m_pExceptionValue{ nullptr };

CProgramRuntime::CProgramRuntime(RuntimeModules&& modules){
	m_oModules = std::move(modules);
	m_bExceptionThrown = false;
	m_pExceptionValue = nullptr;
	m_pCodePosition = nullptr;
}

CProgramRuntime::~CProgramRuntime(){};

using steady_clock = std::chrono::time_point<std::chrono::steady_clock>;

template<typename T>
concept PrintableValue = VariableT<T> || IValueChild<T>;

template<PrintableValue T>
void PrintLeaks(const char* name) {
	if(const auto count = CProgramRuntime::GetPool<T>().GetInUseCount())
		std::cout << std::format("LEAK -> {}: {}\n", name, count);
}

template<PrintableValue T>
bool HasLeak() {
	if (const auto count = CProgramRuntime::GetPool<T>().GetInUseCount())
		return true;

	return false;
}

#ifdef RUNNING_TESTS
IValue* CProgramRuntime::Execute()
{
	m_pExceptionValue = nullptr;
	CRuntimeFunction* mainFunction = FindMainFunction(m_oModules);

	if (!mainFunction) {
		throw CRuntimeError("couldn't find the \"main\" function");
	}

	for (auto& mod : m_oModules) {
		BuiltInMethods::Setup(mod->GetContext());

		mod->SetupGlobalVariables();
		mod->EvaluateGlobalExpressions();
	}

	IValue* returnValue = BeginExecution(mainFunction);

	for (auto& mod : m_oModules)
		mod->FreeGlobalVariables();

	return returnValue;
}

#else
void CProgramRuntime::Execute()
{
	m_pExceptionValue = nullptr;
	CRuntimeFunction* mainFunction = FindMainFunction(m_oModules);

	if (!mainFunction) {
		throw CRuntimeError("couldn't find the \"main\" function");
	}

	for (auto& mod : m_oModules) {
		BuiltInMethods::Setup(mod->GetContext());

		mod->SetupGlobalVariables();
		mod->EvaluateGlobalExpressions();
	}

	const steady_clock old = std::chrono::steady_clock::now();
	const steady_clock now = BeginExecution(mainFunction);

	const std::chrono::duration<float> difference = now - old;
	std::cout << std::format("\ntime taken: {:.6f}s\n", difference.count());

	for (auto& mod : m_oModules) 
		mod->FreeGlobalVariables();

	PrintLeaks<IValue>        ("undefined");
	PrintLeaks<CBooleanValue> ("boolean");
	PrintLeaks<CIntValue>     ("int");
	PrintLeaks<CDoubleValue>  ("double");
	PrintLeaks<CStringValue>  ("string");
	PrintLeaks<CCallableValue>("callable");
	PrintLeaks<CArrayValue>   ("array");
	PrintLeaks<CObjectValue>  ("object");
	PrintLeaks<CVariable>     ("variable");
}

#endif

#ifdef RUNNING_TESTS
IValue* CProgramRuntime::BeginExecution(CRuntimeFunction* entryFunc)
#else
steady_clock CProgramRuntime::BeginExecution(CRuntimeFunction* entryFunc)
#endif
{
	assert(entryFunc);

	CRuntimeContext ctx{
		.m_pModule = CProgramRuntime::GetModuleByIndex(entryFunc->GetModuleIndex()),
		.m_pFunction = nullptr,
	};

	std::vector<IValue*> args;
	if (auto returnValue = entryFunc->Execute(&ctx, args, {})) {
#ifdef RUNNING_TESTS
		return returnValue;
#else
		steady_clock now = std::chrono::steady_clock::now();
		std::cout << std::format("The program returned: {}\n", returnValue->ToPrintableString());
		returnValue->Release();
		return now;
#endif
	}
	
#ifdef RUNNING_TESTS
	return CProgramRuntime::AcquireNewValue<IValue>();
#else
	return std::chrono::steady_clock::now();
#endif

}
CRuntimeFunction* CProgramRuntime::FindMainFunction(const RuntimeModules& modules)
{
	for (auto& mod : modules) {

		const auto iMainFunction = std::ranges::find(mod->m_oFunctions, "main", [](const RuntimeFunction& rf) { return rf->GetName(); });

		if (iMainFunction != mod->m_oFunctions.end()) {
			return iMainFunction->get();
		}
	}

	return nullptr;
}
void CProgramRuntime::SetExecutionPosition(const CodePosition* pos) noexcept{
	m_pCodePosition = pos;
}
const CodePosition* CProgramRuntime::GetExecutionPosition() noexcept{
	return m_pCodePosition;
}
CRuntimeModule* CProgramRuntime::GetModuleByIndex(std::size_t index) { 
	assert(index < m_oModules.size());
	return m_oModules[index].get(); 
}
bool CProgramRuntime::HasLeaks()
{
	return (
		HasLeak<IValue>() ||
		HasLeak<CBooleanValue>() ||
		HasLeak<CIntValue>() ||
		HasLeak<CDoubleValue>() ||
		HasLeak<CStringValue>() ||
		HasLeak<CCallableValue>() ||
		HasLeak<CArrayValue>() ||
		HasLeak<CObjectValue>() ||
		HasLeak<CVariable>() || 
		HasLeak<CConsoleValue>()
	);
}
