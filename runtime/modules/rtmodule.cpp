#include "rtmodule.hpp"

#include "api/types/internal/objects.hpp"

#include "linter/modules/module.hpp"

#include "api/internal/runtime.hpp"
#include "api/internal/variables.hpp"
#include "api/internal/structure.hpp"

#include <ranges>
#include <runtime/exceptions/exception.hpp>

CRuntimeModule::CRuntimeModule(CModule& ctx) :
	m_oGlobalScopeInstructions(std::move(ctx.m_oGlobalScopeInstructions)),
	m_uNumGlobalVariables(ctx.m_uNumGlobalVariables), 
	m_oFunctions(std::move(ctx.m_oFunctions)),
	m_oContext(std::move(ctx.m_oContext)),
	m_uModuleIndex(ctx.GetIndex()){}

CRuntimeModule::~CRuntimeModule() = default;


void CRuntimeModule::SetupGlobalVariables(CProgramRuntime* const runtime) {

	if (!runtime->m_pInformation || !runtime->m_pInformation->m_oBuiltInObjects) {
		throw CRuntimeError(runtime, "no runtime context!");
	}
		
	//create global variables
	m_oGlobalVariables = runtime->AcquireNewVariables(m_uNumGlobalVariables);

	std::vector<std::pair<BuiltInMethod_t, BuiltInProperty_t>> classData;

	auto info = runtime->m_pInformation.get();
	for (auto& [_, v] : info->m_oBuiltInObjects->Iterator()) {
		const auto& [methodCtor, propertyCtor] = v;

		BuiltInMethod_t methods = methodCtor ? (*methodCtor)(info) : BuiltInMethod_t(info);
		BuiltInProperty_t properties = propertyCtor ? (*propertyCtor)(info) : BuiltInProperty_t(info);

		classData.emplace_back(std::move(methods), std::move(properties));
	}

	assert(m_oGlobalVariables.size() >= classData.size());

	for (size_t i{}; auto& [method, property] : classData) {
		m_oGlobalVariables[i]->SetValue(CBuiltInObject::Construct(runtime, std::move(method), std::move(property)));
		m_oGlobalVariables[i]->GetValue()->MakeImmutable();
		i++;
	}

	for (auto& var : m_oGlobalVariables | std::views::drop(classData.size())) {
		var->SetValue(IValue::Construct(runtime));
	}
}
void CRuntimeModule::EvaluateGlobalExpressions(CProgramRuntime* const runtime) {

	CRuntimeContext ctx{
		.m_pRuntime = runtime,
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
CRuntimeFunctionBase* CRuntimeModule::GetFunctionByIndex(std::size_t index)
{
	assert(index < m_oFunctions.size());
	return m_oFunctions[index].get();
}
CVariable* CRuntimeModule::GetGlobalVariableByIndex(std::size_t index)
{
	assert(index < m_oGlobalVariables.size());
	return m_oGlobalVariables[index];
}
