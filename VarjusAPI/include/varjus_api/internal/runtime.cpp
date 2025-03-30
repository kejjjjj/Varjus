#include "varjus_api/internal/structure.hpp"
#include "varjus_api/types/internal/objects.hpp"
#include "varjus_api/types/types.hpp"

#include "runtime.hpp"
#include "varjus_api/internal/exceptions/exception.hpp"
#include "runtime/modules/rtmodule.hpp"

#include "variables.hpp"

#include <chrono>
#include <iostream>
#include <ranges>

#define VALUEPOOL_INIT_SIZE size_t(100)

CProgramRuntime::CProgramRuntime(std::unique_ptr<CProgramInformation>&& info, RuntimeModules&& modules)
	: m_pInformation(std::move(info)), 
	m_oDefaultObjects(
		CDefaultObject<CStringValue>(CStringValue::ConstructMethods(m_pInformation.get()), CStringValue::ConstructProperties(m_pInformation.get())),
		CDefaultObject<CArrayValue>(CArrayValue::ConstructMethods(m_pInformation.get()), CArrayValue::ConstructProperties(m_pInformation.get())),
		CDefaultObject<CObjectValue>(CObjectValue::ConstructMethods(m_pInformation.get()), CObjectValue::ConstructProperties(m_pInformation.get()))),
	m_oModules(std::move(modules)) {	

	AllocatePools(VALUEPOOL_INIT_SIZE);
}

CProgramRuntime::~CProgramRuntime(){};

template<typename T>
concept PrintableValue = VariableT<T> || IValueChild<T>;

template<PrintableValue T>
void PrintLeaks(CProgramRuntime* _this, const VarjusString& name) {
	if(const auto count = _this->GetPool<T>().GetInUseCount())
		fmt::print(std::cerr, VSL("LEAK -> {}: {}\n"), name, count);
}

template<PrintableValue T>
bool HasLeak(CProgramRuntime* _this) {
	if (const auto count = _this->GetPool<T>().GetInUseCount())
		return true;

	return false;
}

template<PrintableValue T>
void ClearPool(CProgramRuntime* _this) {
	_this->GetPool<T>().ResetPool();
}

IValue* CProgramRuntime::Execute(IValues& args)
{
	m_pExceptionValue = nullptr;
	CRuntimeFunction* mainFunction = FindMainFunction(m_oModules);

	if (!mainFunction) {
		throw CRuntimeError(this, VSL("couldn't find the \"main\" function"));
	}

	for (auto& mod : m_oModules) {
		mod->SetupGlobalVariables(this);
		mod->EvaluateGlobalExpressions(this);
	}

	IValue* returnValue = BeginExecution(mainFunction, args);

	for (auto& mod : m_oModules)
		mod->FreeGlobalVariables();


	return returnValue;
}


IValue* CProgramRuntime::BeginExecution(CRuntimeFunction* entryFunc, IValues& args)
{
	assert(entryFunc);

	CRuntimeContext ctx{
		.m_pRuntime = this,
		.m_pModule = GetModuleByIndex(entryFunc->GetModuleIndex()),
		.m_pFunction = nullptr,
	};

	if (auto returnValue = entryFunc->Execute(&ctx, nullptr, args, {})) {
		return returnValue;
	}
	
	return IValue::Construct(this);
}
void CProgramRuntime::Cleanup()
{
	FreeAllPools();



}
CRuntimeFunction* CProgramRuntime::FindMainFunction(const RuntimeModules& modules)
{
	for (auto& mod : modules) {

		const auto iMainFunction = std::ranges::find(mod->m_oFunctions, VSL("main"), [](const RuntimeFunction& rf) { return rf->GetName(); });

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
		HasLeak<IValue>(this) ||
		HasLeak<CBooleanValue>(this) ||
		HasLeak<CIntValue>(this) ||
		HasLeak<CUIntValue>(this) ||
		HasLeak<CDoubleValue>(this) ||
		HasLeak<CStringValue>(this) ||
		HasLeak<CCallableValue>(this) ||
		HasLeak<CArrayValue>(this) ||
		HasLeak<CObjectValue>(this) ||
		HasLeak<CVariable>(this) ||
		HasLeak<CBuiltInObject>(this)
	);
}
void CProgramRuntime::PrintAllLeaks()
{
	PrintLeaks<IValue>(this, VSL("undefined"));
	PrintLeaks<CBooleanValue>(this, VSL("boolean"));
	PrintLeaks<CIntValue>(this, VSL("int"));
	PrintLeaks<CUIntValue>(this, VSL("uint"));
	PrintLeaks<CDoubleValue>(this, VSL("VarjusDouble"));
	PrintLeaks<CStringValue>(this, VSL("string"));
	PrintLeaks<CCallableValue>(this, VSL("callable"));
	PrintLeaks<CArrayValue>(this, VSL("array"));
	PrintLeaks<CObjectValue>(this, VSL("object"));
	PrintLeaks<CVariable>(this, VSL("variable"));
	PrintLeaks<CBuiltInObject>(this, VSL("built-in object"));
}
void CProgramRuntime::FreeAllPools()
{
	ClearPool<IValue>(this);
	ClearPool<CBooleanValue>(this);
	ClearPool<CIntValue>(this);
	ClearPool<CUIntValue>(this);
	ClearPool<CDoubleValue>(this);
	ClearPool<CStringValue>(this);
	ClearPool<CCallableValue>(this);
	ClearPool<CArrayValue>(this);
	ClearPool<CObjectValue>(this);
	ClearPool<CVariable>(this);
	ClearPool<CBuiltInObject>(this);
}