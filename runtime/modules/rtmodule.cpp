#include "rtmodule.hpp"

#include "linter/modules/module.hpp"

#include "runtime/runtime.hpp"
#include "runtime/variables.hpp"
#include "runtime/structure.hpp"
#include "runtime/values/types/internal_objects/console.hpp"
#include "runtime/values/types/internal_objects/math.hpp"
#include "runtime/values/types/internal_objects/internal_objects.hpp"

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

	std::vector<BuiltInMethod_t> methods;
	methods.emplace_back(CConsoleValue::ConstructMethods());
	methods.emplace_back(CMathValue::ConstructMethods());

	assert(methods.size() == rto_count);
	assert(m_oGlobalVariables.size() >= methods.size());

	for (size_t i{}; auto& method : methods) {
		m_oGlobalVariables[i]->SetValue(CBuiltInObject::Construct(std::move(method)));
		m_oGlobalVariables[i]->GetValue()->MakeImmutable();
		i++;
	}

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
