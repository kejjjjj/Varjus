#include "module.hpp"

#include "runtime/structure.hpp"
#include "runtime/modules/rtmodule.hpp"
#include "exports/export.hpp"

#include <ranges>

CModule::CModule() = default;
CModule::CModule(const std::string& filePath) : m_oContext(filePath) {}
CModule::~CModule() = default;


void CModule::AddFunction(RuntimeFunction&& func) {
	m_oFunctions.emplace_back(std::move(func)); 
}
CRuntimeFunction* CModule::FindFunction(const std::string& v) const
{
	const auto it = std::ranges::find(m_oFunctions, v, [](const RuntimeFunction& v) { return v->GetName(); });
	return it != m_oFunctions.end() ? it->get() : nullptr;
}
size_t CModule::GetFunctionCount() const noexcept {
	return m_oFunctions.size();
}

void CModule::AddGlobalInstructions(VectorOf<RuntimeBlock>&& insns) {
	m_oGlobalScopeInstructions = std::move(insns);
}
void CModule::SetGlobalVariableCount(std::size_t v) { 
	m_uNumGlobalVariables = v; 
}

std::unique_ptr<CRuntimeModule> CModule::ToRuntimeModule(){
	return std::make_unique<CRuntimeModule>(*this);
}
void CModule::AddExport(const std::string& name, UniqueExportedSymbol&& symbol) {
	m_oModuleExports[name] = std::move(symbol);
}
CExportedSymbol* CModule::GetExport(const std::string& name) const {
	if (!m_oModuleExports.contains(name))
		return nullptr;

	return m_oModuleExports.at(name).get();
}

/***********************************************************************
 > 
***********************************************************************/
VectorOf<std::unique_ptr<CModule>> CModule::m_oAllModules;
std::unordered_map<std::string, CModule*> CModule::m_oCachedModules;


CModule* CModule::CreateNewModule(const std::string& filePath)
{
	auto index = m_oAllModules.size();
	auto& thisModule = m_oAllModules.emplace_back(std::make_unique<CModule>(filePath));
	thisModule->m_uIndex = index;
	m_oCachedModules[filePath] = thisModule.get();
	return thisModule.get();
}
CModule* CModule::FindCachedModule(const std::string& filePath)
{
	if (!m_oCachedModules.contains(filePath))
		return nullptr;

	return m_oCachedModules.at(filePath);
}
RuntimeModules CModule::ToRuntimeModules()
{
	RuntimeModules modules;
	for (auto& tMod : m_oAllModules) {
		modules.emplace_back(std::move(tMod->ToRuntimeModule()));
	}

	m_oAllModules.clear();
	return modules;
}




