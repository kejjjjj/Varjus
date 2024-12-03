#include "runtime.hpp"
#include "structure.hpp"
#include "values/types/array_internal/array_builtin.hpp"
#include "values/types/types.hpp"
#include "variables.hpp"

#include <ranges>
#include <iostream>
#include <chrono>
#include "exceptions/exception.hpp"

#define VALUEPOOL_INIT_SIZE size_t(100)

//CNonOwningObjectPool<CVariable>                  CProgramRuntime::m_oVariablePool                  (VALUEPOOL_INIT_SIZE);

template<> COwningObjectPool<CVariable>           CProgramRuntime::m_oValuePool<CVariable>           (VALUEPOOL_INIT_SIZE);
template<> COwningObjectPool<IValue>              CProgramRuntime::m_oValuePool<IValue>              (VALUEPOOL_INIT_SIZE);
template<> COwningObjectPool<CBooleanValue>       CProgramRuntime::m_oValuePool<CBooleanValue>       (VALUEPOOL_INIT_SIZE);
template<> COwningObjectPool<CIntValue>           CProgramRuntime::m_oValuePool<CIntValue>           (VALUEPOOL_INIT_SIZE);
template<> COwningObjectPool<CDoubleValue>        CProgramRuntime::m_oValuePool<CDoubleValue>        (VALUEPOOL_INIT_SIZE);
template<> COwningObjectPool<CStringValue>        CProgramRuntime::m_oValuePool<CStringValue>        (VALUEPOOL_INIT_SIZE);
template<> COwningObjectPool<CCallableValue>      CProgramRuntime::m_oValuePool<CCallableValue>      (VALUEPOOL_INIT_SIZE);
template<> COwningObjectPool<CArrayValue>         CProgramRuntime::m_oValuePool<CArrayValue>         (VALUEPOOL_INIT_SIZE);
template<> COwningObjectPool<CObjectValue>        CProgramRuntime::m_oValuePool<CObjectValue>        (VALUEPOOL_INIT_SIZE);

VectorOf<RuntimeFunction> CProgramRuntime::m_oFunctions;
VectorOf<CVariable*> CProgramRuntime::m_oGlobalVariables;
CProgramContext* CProgramRuntime::m_pContext{ nullptr };
const CodePosition* CProgramRuntime::m_pCodePosition{ nullptr };

CProgramRuntime::CProgramRuntime(CFileRuntimeData* const file, CProgramContext* const context)
{
	m_pContext = context;
	m_oFunctions = (std::move(file->m_oFunctions));
	m_oGlobalScopeInstructions = std::move(file->m_oGlobalScopeInstructions);
	m_uNumGlobalVariables = file->m_uNumGlobalVariables;
	m_oGlobalVariables.clear();

	assert(m_pContext);
}
CProgramRuntime::~CProgramRuntime() { m_oFunctions.clear(); };

using steady_clock = std::chrono::time_point<std::chrono::steady_clock>;

void CProgramRuntime::Execute()
{

	const auto iMainFunction = std::ranges::find(m_oFunctions, "main", [](const RuntimeFunction& rf) { return rf->GetName(); });


	if (iMainFunction == m_oFunctions.end()) {
		throw CRuntimeError("couldn't find the \"main\" function");
	}

	CStaticArrayBuiltInMethods::Initialize(GetContext());

	//create global variables
	m_oGlobalVariables = CProgramRuntime::AcquireNewVariables(m_uNumGlobalVariables);

	for (auto& var : m_oGlobalVariables) {
		var->SetValue(AcquireNewValue<IValue>());
	}

	for (auto& insn : m_oGlobalScopeInstructions) {
		if (insn->Execute(nullptr))
			break;
	}

	steady_clock old = std::chrono::steady_clock::now();
	steady_clock now;

	std::vector<IValue*> args;
	if (auto v = (*iMainFunction)->Execute(nullptr, args, {})) {
		now = std::chrono::steady_clock::now();
		std::cout << v->ToPrintableString() << '\n';
		v->Release();

	} else {
		now = std::chrono::steady_clock::now();
	}

	std::chrono::duration<float> difference = now - old;
	printf("\ntime taken: %.6f\n", difference.count());

	for (auto& variable : m_oGlobalVariables)
		variable->Release();

	std::cout << "\n\n--------------LEAKS--------------\n\n";
	std::cout << std::format("undefined: {}\n",   GetPool<IValue>().GetInUseCount());
	std::cout << std::format("boolean:   {}\n",   GetPool<CBooleanValue>().GetInUseCount());
	std::cout << std::format("int:       {}\n",   GetPool<CIntValue>().GetInUseCount());
	std::cout << std::format("double:    {}\n",   GetPool<CDoubleValue>().GetInUseCount());
	std::cout << std::format("string:    {}\n",   GetPool<CStringValue>().GetInUseCount());
	std::cout << std::format("callable:  {}\n",   GetPool<CCallableValue>().GetInUseCount());
	std::cout << std::format("array:     {}\n",   GetPool<CArrayValue>().GetInUseCount());
	std::cout << std::format("object:    {}\n",   GetPool<CObjectValue>().GetInUseCount());
	std::cout << std::format("variable:  {}\n\n", GetPool<CVariable>().GetInUseCount());

}
CProgramContext* CProgramRuntime::GetContext()
{
	return m_pContext;
}
CRuntimeFunction* CProgramRuntime::GetFunctionByIndex(std::size_t index)
{
	assert(index < m_oFunctions.size());
	return m_oFunctions[index].get();
}
CVariable* CProgramRuntime::GetGlobalVariableByIndex(std::size_t index)
{
	assert(index < m_oGlobalVariables.size());
	return m_oGlobalVariables[index];
}
void CProgramRuntime::SetExecutionPosition(const CodePosition* pos) noexcept{
	m_pCodePosition = pos;
}
const CodePosition* CProgramRuntime::GetExecutionPosition() noexcept{
	return m_pCodePosition;
}
