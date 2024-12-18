#include "module.hpp"

#include "runtime/structure.hpp"
#include "runtime/modules/rtmodule.hpp"
#include "exports/export.hpp"

#include <ranges>
#include <sstream>
#include <functional>
#include <linter/error.hpp>

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
DependencyGraph CModule::m_oDependencyGraph;
std::unordered_set<std::string> CModule::m_oVisitedModules;

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

void CModule::CheckCircularDependencies(const std::string& src, const DependencyGraph& graph)
{
	if (graph.empty())
		return;

	std::unordered_set<std::string> visited;
	VectorOf<std::string> recursionStack;
	VectorOf<std::pair<std::string, std::string>> conflictDetails;

	std::function<bool(const std::string&)> DFS = [&](const std::string& amodule) {
		
		visited.insert(amodule);
		recursionStack.push_back(amodule);

		if (graph.contains(amodule)) {
			for (const auto& dependency : graph.at(amodule)) {

				if (std::find(recursionStack.begin(), recursionStack.end(), dependency) != recursionStack.end()) {

					for(auto i : std::views::iota(0u, recursionStack.size() - 1))
						conflictDetails.emplace_back(recursionStack[i], recursionStack[i + 1]);
					
					return false;

				}

				if (!visited.contains(dependency))
					return DFS(dependency);
			}
		}

		recursionStack.pop_back();
		return true;
	};

	if (!DFS(src)) {

		std::stringstream ss;
		ss << "circular dependency detected involving the following files:\n";
		for (const auto& [source, target] : conflictDetails)
			ss << " - " << source << " <-> " << target << '\n';

		return CLinterErrors::PushError(ss.str());
	}
}

std::string CModule::DependencyGraphToString() noexcept
{
	std::stringstream ss;

	for (const auto& [sourceFile, dependencies] : CModule::m_oDependencyGraph) {
		ss << sourceFile << " imports:\n";

		if (dependencies.empty()) {
			ss << " - None\n"; // Handle case where there are no imports
		}
		else {
			for (const auto& dependency : dependencies) {
				ss << " - " << dependency << '\n';
			}
		}
	}

	return ss.str();
}

