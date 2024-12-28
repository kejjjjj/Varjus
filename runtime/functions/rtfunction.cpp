#include "rtfunction.hpp"
#include "runtime/runtime.hpp"
#include "runtime/structure.hpp"
#include "runtime/variables.hpp"
#include "runtime/values/types/types.hpp"
#include "runtime/modules/rtmodule.hpp"

#include "linter/functions/function.hpp"

#include <ranges>
#include <algorithm>

#include <iostream>
#include <runtime/exceptions/exception.hpp>

CRuntimeFunction::CRuntimeFunction(ElementIndex moduleIndex, CFunctionBlock& linterFunction,
	VectorOf<CCrossModuleReference>&& args,
	VectorOf<CCrossModuleReference>&& variableIndices) :

	IRuntimeStructureSequence(std::move(linterFunction.m_oInstructions)),
	m_uModuleIndex(moduleIndex),
	m_sName(linterFunction.m_sName),
	m_uNumParameters(linterFunction.m_uNumParameters),
	m_uNumVariables(linterFunction.m_pStack->m_VariableManager->GetVariableCount()),
	m_oArgumentIndices(std::move(args)),
	m_oVariableIndices(std::move(variableIndices))
{

}
CRuntimeFunction::~CRuntimeFunction() = default;

IValue* CRuntimeFunction::Execute(CRuntimeContext* const ctx, IValue* _this,
	VectorOf<IValue*>& args, const VariableCaptures& captures)
{
	if (m_uNumParameters != args.size())
		throw CRuntimeError(std::format("the callable expected {} arguments instead of {}", m_uNumParameters, args.size()));


	auto func = CFunction(_this, args, captures, *this);
	const auto isMainFunction = ctx->m_pFunction == nullptr;

	CRuntimeContext thisContext{
		.m_pModule = ctx->m_pModule,
		.m_pFunction = &func
	};

	IValue* returnVal{ nullptr };

	for (const auto& insn : m_oInstructions) {
		if (returnVal = insn->Execute(&thisContext), returnVal) {

			if (isMainFunction && CProgramRuntime::ExceptionThrown())
				throw CRuntimeError(std::format("an uncaught exception: {}", returnVal->ToPrintableString()));

			break;
		}

		//assert(!CProgramRuntime::ExceptionThrown());
	}

	IValue* copy = nullptr;

	if (returnVal) {
		copy = returnVal->HasOwner() ? returnVal->Copy() : returnVal;
	} else {
		copy = CProgramRuntime::AcquireNewValue<IValue>();
	}

	for (auto& [index, variable] : func.m_oStack) {

		//did this function capture this?
		//don't free captured values or they get destroyed and the next calls to this function fail
		if(!captures.contains(index))
			variable->Release();
	}

	return copy;
}

CBuiltInRuntimeFunction::CBuiltInRuntimeFunction(METHOD_AS_VARIABLE(method), std::size_t numArgs)
	: m_pMethod(method), m_uNumArguments(numArgs) {}
CBuiltInRuntimeFunction::~CBuiltInRuntimeFunction() = default;

IValue* CBuiltInRuntimeFunction::ExecuteFunction( CRuntimeContext* const ctx, IValue* _this,
	VectorOf<IValue*>& args, [[maybe_unused]] const VariableCaptures& captures)
{

	assert(m_pMethod);

	if (m_uNumArguments != UNCHECKED_PARAMETER_COUNT && m_uNumArguments != args.size())
		throw CRuntimeError(std::format("the method expected {} arguments instead of {}", m_uNumArguments, args.size()));

	auto returnVal = m_pMethod(ctx, _this, args);

	for(auto& val : args)
		val->Release();

	return returnVal;
}

CFunction::CFunction(IValue* _this, VectorOf<IValue*>& args,
	const VariableCaptures& captures, const CRuntimeFunction& func)
	: m_pThis(_this)
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

CVariable* CFunction::GetVariableByRef(const CCrossModuleReference& ref) const
{
	assert(m_oStack.contains(ref));
	return m_oStack.at(ref);
}