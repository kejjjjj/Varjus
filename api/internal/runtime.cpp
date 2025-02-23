#include "api/internal/structure.hpp"
#include "api/types/internal/objects.hpp"
#include "api/types/types.hpp"

#include "runtime.hpp"
#include "runtime/exceptions/exception.hpp"
#include "runtime/modules/rtmodule.hpp"

#include "variables.hpp"

#include <chrono>
#include <iostream>
#include <ranges>

#define VALUEPOOL_INIT_SIZE size_t(100)

template<> COwningObjectPool<CVariable>           CProgramRuntime::m_oValuePool<CVariable>{}; 
template<> COwningObjectPool<IValue>              CProgramRuntime::m_oValuePool<IValue>{};
template<> COwningObjectPool<CBooleanValue>       CProgramRuntime::m_oValuePool<CBooleanValue>{};
template<> COwningObjectPool<CIntValue>           CProgramRuntime::m_oValuePool<CIntValue>{};
template<> COwningObjectPool<CUIntValue>          CProgramRuntime::m_oValuePool<CUIntValue>{};
template<> COwningObjectPool<CDoubleValue>        CProgramRuntime::m_oValuePool<CDoubleValue>{};
template<> COwningObjectPool<CStringValue>        CProgramRuntime::m_oValuePool<CStringValue>{};
template<> COwningObjectPool<CCallableValue>      CProgramRuntime::m_oValuePool<CCallableValue>{};
template<> COwningObjectPool<CArrayValue>         CProgramRuntime::m_oValuePool<CArrayValue>{};
template<> COwningObjectPool<CObjectValue>        CProgramRuntime::m_oValuePool<CObjectValue>{};
template<> COwningObjectPool<CBuiltInObject>      CProgramRuntime::m_oValuePool<CBuiltInObject>{};

RuntimeModules CProgramRuntime::m_oModules;
const CodePosition* CProgramRuntime::m_pCodePosition{ nullptr };
bool CProgramRuntime::m_bExceptionThrown{ false };
IValue* CProgramRuntime::m_pExceptionValue{ nullptr };

CProgramRuntime::CProgramRuntime(RuntimeModules&& modules){
	FreeAllPools();
	AllocatePools(VALUEPOOL_INIT_SIZE);

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

template<PrintableValue T>
void ClearPool() {
	CProgramRuntime::GetPool<T>().ResetPool();
}

static void InitGlobals()
{
	// these are static because otherwise using objects, arrays, and strings would be quite expensive
	// as the methods and properties would have to be copied everytime
	CArrayValue::ConstructMethods();
	CArrayValue::ConstructProperties();

	CObjectValue::ConstructMethods();
	CObjectValue::ConstructProperties();

	CStringValue::ConstructMethods();
	CStringValue::ConstructProperties();
}

IValue* CProgramRuntime::Execute()
{
	m_pExceptionValue = nullptr;
	CRuntimeFunction* mainFunction = FindMainFunction(m_oModules);

	if (!mainFunction) {
		throw CRuntimeError("couldn't find the \"main\" function");
	}

	InitGlobals();

	for (auto& mod : m_oModules) {
		mod->SetupGlobalVariables();
		mod->EvaluateGlobalExpressions();
	}

	IValue* returnValue = BeginExecution(mainFunction);

	for (auto& mod : m_oModules)
		mod->FreeGlobalVariables();


	return returnValue;
}


IValue* CProgramRuntime::BeginExecution(CRuntimeFunction* entryFunc)
{
	assert(entryFunc);

	CRuntimeContext ctx{
		.m_pModule = CProgramRuntime::GetModuleByIndex(entryFunc->GetModuleIndex()),
		.m_pFunction = nullptr,
	};

	std::vector<IValue*> args;
	if (auto returnValue = entryFunc->Execute(&ctx, nullptr, args, {})) {
		return returnValue;
	}
	
	return IValue::Construct();
}
void CProgramRuntime::Cleanup()
{
	FreeAllPools();
}
CRuntimeFunction* CProgramRuntime::FindMainFunction(const RuntimeModules& modules)
{
	for (auto& mod : modules) {

		const auto iMainFunction = std::ranges::find(mod->m_oFunctions, "main", [](const RuntimeFunction& rf) { return rf->GetName(); });

		if (iMainFunction != mod->m_oFunctions.end() && (*iMainFunction)->FunctionType() == fn_regular) {
			return dynamic_cast<CRuntimeFunction*>(iMainFunction->get());
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
		HasLeak<CUIntValue>() ||
		HasLeak<CDoubleValue>() ||
		HasLeak<CStringValue>() ||
		HasLeak<CCallableValue>() ||
		HasLeak<CArrayValue>() ||
		HasLeak<CObjectValue>() ||
		HasLeak<CVariable>() || 
		HasLeak<CBuiltInObject>()
	);
}
void CProgramRuntime::PrintAllLeaks()
{
	PrintLeaks<IValue>("undefined");
	PrintLeaks<CBooleanValue>("boolean");
	PrintLeaks<CIntValue>("int");
	PrintLeaks<CUIntValue>("uint");
	PrintLeaks<CDoubleValue>("VarjusDouble");
	PrintLeaks<CStringValue>("string");
	PrintLeaks<CCallableValue>("callable");
	PrintLeaks<CArrayValue>("array");
	PrintLeaks<CObjectValue>("object");
	PrintLeaks<CVariable>("variable");
	PrintLeaks<CBuiltInObject>("built-in object");
}
void CProgramRuntime::FreeAllPools()
{
	ClearPool<IValue>();
	ClearPool<CBooleanValue>();
	ClearPool<CIntValue>();
	ClearPool<CUIntValue>();
	ClearPool<CDoubleValue>();
	ClearPool<CStringValue>();
	ClearPool<CCallableValue>();
	ClearPool<CArrayValue>();
	ClearPool<CObjectValue>();
	ClearPool<CVariable>();
	ClearPool<CBuiltInObject>();
}
void CProgramRuntime::AllocatePools(std::size_t initialSize)
{
	CProgramRuntime::Cleanup();
	CProgramRuntime::m_oValuePool<CVariable>.Grow(initialSize);
	CProgramRuntime::m_oValuePool<IValue>.Grow(initialSize);
	CProgramRuntime::m_oValuePool<CBooleanValue>.Grow(initialSize);
	CProgramRuntime::m_oValuePool<CIntValue>.Grow(initialSize);
	CProgramRuntime::m_oValuePool<CUIntValue>.Grow(initialSize);
	CProgramRuntime::m_oValuePool<CDoubleValue>.Grow(initialSize);
	CProgramRuntime::m_oValuePool<CStringValue>.Grow(initialSize);
	CProgramRuntime::m_oValuePool<CCallableValue>.Grow(initialSize);
	CProgramRuntime::m_oValuePool<CArrayValue>.Grow(initialSize);
	CProgramRuntime::m_oValuePool<CObjectValue>.Grow(initialSize);
	CProgramRuntime::m_oValuePool<CBuiltInObject>.Grow(initialSize);
}