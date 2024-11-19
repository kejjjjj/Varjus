#include "rtfunction.hpp"
#include "runtime/runtime.hpp"
#include "runtime/structure.hpp"
#include "runtime/variables.hpp"
#include "runtime/values/types/types.hpp"

#include "linter/functions/function.hpp"

#include <ranges>
#include <algorithm>

#include <iostream>
#include <runtime/exceptions/exception.hpp>

CRuntimeFunction::CRuntimeFunction(CFunctionBlock& linterFunction,
	VectorOf<VariableIndex>&& args,
	VectorOf<VariableIndex>&& variableIndices) :

	IRuntimeStructureSequence(std::move(linterFunction.m_oInstructions)),
	m_sName(linterFunction.m_sName),
	m_uNumParameters(linterFunction.m_uNumParameters),
	m_uNumVariables(linterFunction.m_pStack->GetVariableCount()),
	m_oArgumentIndices(std::move(args)),
	m_oVariableIndices(std::move(variableIndices))
{

}
CRuntimeFunction::~CRuntimeFunction() = default;

IValue* CRuntimeFunction::Execute([[maybe_unused]] CFunction* const thisFunction, VectorOf<IValue*>& args, 
	const VariableCaptures& captures)
{
	if (m_uNumParameters != args.size())
		throw CRuntimeError(std::format("the callable expected {} arguments instead of {}", m_uNumParameters, args.size()));

	auto func = CFunction(args, captures, *this);

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

	for (auto& [index, value] : func.m_oStack) {

		if (!thisFunction) {
			assert(value->GetValue());
			std::cout << value->GetValue()->ToPrintableString() << '\n';
		}
		value->Release();
	}

	return copy;
}

CFunction::CFunction(VectorOf<IValue*>& args, const VariableCaptures& captures, const CRuntimeFunction& func)
{

	for (auto i = std::size_t(0); auto& arg : func.m_oArgumentIndices) {
		auto var = m_oStack[arg] = CProgramRuntime::AcquireNewVariable();
		var->SetValue(args[i++]);
	}

	for (auto& v : func.m_oVariableIndices) {
		auto var = m_oStack[v] = CProgramRuntime::AcquireNewVariable();
		assert(var->GetValue() == nullptr);
		var->SetValue(CProgramRuntime::AcquireNewValue<IValue>());
	}

	for (auto& [key, value] : captures)
		m_oStack[key] = value;
	
}

CVariable* CFunction::GetVariableByIndex(std::size_t index) const
{
	return m_oStack.at(index);
}