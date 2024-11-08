#include "rtfunction.hpp"
#include "runtime/runtime.hpp"
#include "runtime/structure.hpp"
#include "runtime/variables.hpp"
#include "runtime/values/types/types.hpp"

#include "linter/functions/function.hpp"

#include <ranges>
#include <algorithm>

#include <chrono>
#include <iostream>
#include <runtime/exceptions/exception.hpp>

CRuntimeFunction::CRuntimeFunction(CFunctionBlock& linterFunction) :
	IRuntimeStructureSequence(std::move(linterFunction.m_oInstructions)),
	m_sName(linterFunction.m_sName),
	m_uNumParameters(linterFunction.m_uNumParameters),
	m_uNumVariables(linterFunction.m_pStack->GetVariableCount())
{

}
CRuntimeFunction::~CRuntimeFunction() = default;

IValue* CRuntimeFunction::Execute([[maybe_unused]] CFunction* const thisFunction, VectorOf<IValue*>& args)
{
	std::chrono::time_point<std::chrono::steady_clock> old = std::chrono::steady_clock::now();

	if (m_uNumParameters != args.size())
		throw CRuntimeError(std::format("the callable expected {} arguments instead of {}", m_uNumParameters, args.size()));

	auto& variablePool = CProgramRuntime::m_oVariablePool;
	auto func = CFunction(args, variablePool.Acquire(m_uNumVariables));

	IValue* returnVal{ nullptr };

	for (const auto& insn : m_oInstructions) {
		if (returnVal = insn->Execute(&func), returnVal) {
			break;
		}
	}

	IValue* copy = nullptr;

	if (returnVal) {
		copy = returnVal->HasOwner() ? returnVal->Copy() : returnVal;
	} else {
		copy = CProgramRuntime::AcquireNewValue<IValue>();
	}

	std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();
	std::chrono::duration<float> difference = now - old;

	std::ranges::for_each(func.m_oStack, [&thisFunction, &variablePool](std::unique_ptr<CVariable>& v) {
		
		auto& value = v->GetValue();

		if(!thisFunction)
			std::cout << value->ToPrintableString() << '\n';

		assert(value->HasOwner());
		value->Release();		
		value = nullptr;
		variablePool.Release(std::move(v)); 

	});

	//printf("\ntime taken: %.6f\n", difference.count());
	return copy;
}

CFunction::CFunction(VectorOf<IValue*>& args, VectorOf<std::unique_ptr<CVariable>>&& variables) : m_oStack(std::move(variables))
{

	for (auto i = size_t(0); i < args.size(); i++) {
		
		assert(m_oStack[i]->GetValue() == nullptr);
		m_oStack[i]->SetValue(args[i]);
	}

	for (auto& v : m_oStack | std::views::drop(args.size()))
		v->SetValue(CProgramRuntime::AcquireNewValue<IValue>());

}

CVariable* CFunction::GetVariableByIndex(std::size_t index) const
{
	return m_oStack[index].get();
}