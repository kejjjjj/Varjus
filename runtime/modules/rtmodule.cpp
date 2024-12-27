#include "rtmodule.hpp"

#include "linter/modules/module.hpp"

#include "runtime/runtime.hpp"
#include "runtime/variables.hpp"
#include "runtime/structure.hpp"
#include "runtime/values/types/internal_objects/console.hpp"
#include "runtime/values/types/internal_objects/math.hpp"

CRuntimeModule::CRuntimeModule(CModule& ctx) :
	m_oGlobalScopeInstructions(std::move(ctx.m_oGlobalScopeInstructions)),
	m_uNumGlobalVariables(ctx.m_uNumGlobalVariables), 
	m_oFunctions(std::move(ctx.m_oFunctions)),
	m_oContext(std::move(ctx.m_oContext)),
	m_uModuleIndex(ctx.GetIndex()){}

CRuntimeModule::~CRuntimeModule() = default;

void CRuntimeModule::SetupGlobalVariables() {

	//create global variables
	m_oGlobalVariables = CProgramRuntime::AcquireNewVariables(m_uNumGlobalVariables);
	assert(m_oGlobalVariables.size() >= rto_count);

	//setup the console object
	m_oGlobalVariables[rto_console]->SetValue(CConsoleValue::Construct());
	m_oGlobalVariables[rto_console]->GetValue()->MakeImmutable();

	m_oGlobalVariables[rto_math]->SetValue(CMathValue::Construct());
	m_oGlobalVariables[rto_math]->GetValue()->MakeImmutable();

	for (auto& var : m_oGlobalVariables | std::views::drop(rto_count)) {
		var->SetValue(CProgramRuntime::AcquireNewValue<IValue>());
	}
}
void CRuntimeModule::EvaluateGlobalExpressions() {

	CRuntimeContext ctx{
		.m_pModule = this,
		.m_pFunction = nullptr
	};

	for (auto& insn : m_oGlobalScopeInstructions) {
		if (insn->Execute(&ctx))
			break;
	}
}
void CRuntimeModule::FreeGlobalVariables() {
	for (auto& variable : m_oGlobalVariables)
		variable->Release();
}
CRuntimeFunction* CRuntimeModule::GetFunctionByIndex(std::size_t index)
{
	assert(index < m_oFunctions.size());
	return m_oFunctions[index].get();
}
CVariable* CRuntimeModule::GetGlobalVariableByIndex(std::size_t index)
{
	assert(index < m_oGlobalVariables.size());
	return m_oGlobalVariables[index];
}
