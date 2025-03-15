#include "varjus_api/types/types.hpp"
#include "rtfunction.hpp"
#include "runtime/modules/rtmodule.hpp"
#include "varjus_api/internal/runtime.hpp"
#include "varjus_api/internal/structure.hpp"
#include "varjus_api/internal/variables.hpp"

#include "linter/functions/function.hpp"

#include "runtime/exceptions/exception.hpp"

#include <algorithm>
#include <ranges>

CRuntimeFunction::CRuntimeFunction(ElementIndex moduleIndex, CFunctionBlock& linterFunction,
	VectorOf<CCrossModuleReference>&& args,
	VectorOf<CCrossModuleReference>&& variableIndices) :
	CRuntimeFunctionBase(linterFunction.m_sName, linterFunction.m_uNumParameters),
	IRuntimeStructureSequence(std::move(linterFunction.m_oInstructions)),
	m_uModuleIndex(moduleIndex),
	m_uNumVariables(linterFunction.m_pStack->m_VariableManager->GetVariableCount()),
	m_oArgumentIndices(std::move(args)),
	m_oVariableIndices(std::move(variableIndices))
{

}
CRuntimeFunction::~CRuntimeFunction() = default;

IValue* CRuntimeFunction::Execute(CRuntimeContext* const ctx, [[maybe_unused]] IValue* _this,
	VectorOf<IValue*>& args, const VariableCaptures& captures)
{
	assert(ctx->m_pRuntime);
	if (m_uNumArguments != args.size())
		throw CRuntimeError(ctx->m_pRuntime, fmt::format(VSL("the callable expected {} arguments instead of {}"), m_uNumArguments, args.size()));


	auto func = CFunction(ctx->m_pRuntime, args, captures, *this);
	const auto isMainFunction = ctx->m_pFunction == nullptr;

	CRuntimeContext thisContext{
		.m_pRuntime = ctx->m_pRuntime,
		.m_pModule = ctx->m_pModule,
		.m_pFunction = &func
	};

	IValue* returnVal{ nullptr };

	for (const auto& insn : m_oInstructions) {
		if (returnVal = insn->Execute(&thisContext), returnVal) {

			if (isMainFunction && thisContext.m_pRuntime->ExceptionThrown())
				throw CRuntimeError(thisContext.m_pRuntime, fmt::format(VSL("an uncaught exception: {}"), returnVal->ToPrintableString()));

			break;
		}
	}

	IValue* copy = nullptr;

	if (returnVal) {
		copy = returnVal->HasOwner() ? returnVal->Copy() : returnVal;
	} else {
		copy = IValue::Construct(thisContext.m_pRuntime);
	}

	for (auto& [index, variable] : func.m_oStack) {

		//did this function capture this?
		//don't free captured values or they get destroyed and the next calls to this function fail
		if(!captures.contains(index))
			variable->Release();
	}

	assert(copy);
	return copy;
}

CBuiltInRuntimeMethod::CBuiltInRuntimeMethod(Method_t method, std::size_t numArgs)
	: CRuntimeFunctionBase(VSL("method"), numArgs), m_pMethod(method) { }

CBuiltInRuntimeMethod::~CBuiltInRuntimeMethod() = default;

IValue* CBuiltInRuntimeMethod::ExecuteFunction( CRuntimeContext* const ctx, IValue* _this,
	VectorOf<IValue*>& args, [[maybe_unused]] const VariableCaptures& captures)
{

	assert(m_pMethod);

	if (m_uNumArguments != UNCHECKED_PARAMETER_COUNT && m_uNumArguments != args.size())
		throw CRuntimeError(ctx->m_pRuntime, fmt::format(VSL("the method expected {} arguments instead of {}"), m_uNumArguments, args.size()));

	auto returnVal = m_pMethod(ctx, _this, args);

	for(auto& val : args)
		val->Release();

	assert(returnVal);
	return returnVal;
}

CBuiltInRuntimeFunction::CBuiltInRuntimeFunction(const VarjusString& name, Function_t function, std::size_t numArgs)
	: CRuntimeFunctionBase(name, numArgs), m_pFunction(function) {
}

CBuiltInRuntimeFunction::~CBuiltInRuntimeFunction() = default;

IValue* CBuiltInRuntimeFunction::ExecuteFunction(CRuntimeContext* const ctx, [[maybe_unused]] IValue* _this,
	VectorOf<IValue*>& args, [[maybe_unused]] const VariableCaptures& captures)
{

	assert(m_pFunction);

	if (m_uNumArguments != UNCHECKED_PARAMETER_COUNT && m_uNumArguments != args.size())
		throw CRuntimeError(ctx->m_pRuntime, fmt::format(VSL("the callable \"{}\" expected {} arguments instead of {}"), m_sName, m_uNumArguments, args.size()));

	auto returnVal = m_pFunction(ctx, args);

	if(!returnVal)
		throw CRuntimeError(ctx->m_pRuntime, fmt::format(VSL("you forgot to return a value from an internal function \"{}\""), m_sName, args.size()));

	for (auto& val : args)
		val->Release();

	assert(returnVal);
	return returnVal;
}

CFunction::CFunction(CProgramRuntime* const runtime, VectorOf<IValue*>& args,
	const VariableCaptures& captures, const CRuntimeFunction& func)
{
	//setup parameters
	for (auto i = std::size_t(0); auto& arg : func.m_oArgumentIndices) {
		m_oStack[arg] = CVariable::Construct(runtime, args[i++]);
	}

	//setup stack
	for (const auto& v : func.m_oVariableIndices) {
		m_oStack[v] = CVariable::Construct(runtime, IValue::Construct(runtime));
		assert(m_oStack[v]->GetValue() != nullptr);
	}

	//setup lambda captures
	for (const auto& [key, value] : captures)
		m_oStack[key] = value;
}

CVariable* CFunction::GetVariableByRef(const CCrossModuleReference& ref) const
{
	assert(m_oStack.contains(ref));
	return m_oStack.at(ref);
}