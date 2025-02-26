#include "module.hpp"

#include "api/internal/structure.hpp"
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
CRuntimeFunctionBase* CModule::FindFunction(const std::string& v) const
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

CProjectModules::CProjectModules() = default;
CProjectModules::~CProjectModules() = default;

CModule* CProjectModules::CreateNewModule(const std::string& filePath)
{
	auto index = m_oAllModules.size();
	auto& thisModule = m_oAllModules.emplace_back(std::make_unique<CModule>(filePath));
	thisModule->SetIndex(index);
	m_oCachedModules[filePath] = thisModule.get();
	return thisModule.get();
}
CModule* CProjectModules::FindCachedModule(const std::string& filePath)
{
	if (!m_oCachedModules.contains(filePath))
		return nullptr;

	return m_oCachedModules.at(filePath);
}
RuntimeModules CProjectModules::ToRuntimeModules()
{
	RuntimeModules modules;
	for (auto& tMod : m_oAllModules) {
		modules.emplace_back(std::move(tMod->ToRuntimeModule()));
	}

	m_oAllModules.clear();
	return modules;
}

std::optional<std::string> CProjectModules::CheckCircularDependencies(const std::string& src, const DependencyGraph& graph)
{
	if (graph.empty())
		return std::nullopt;

	std::unordered_set<std::string> visited;
	VectorOf<std::string> recursionStack;
	VectorOf<std::pair<std::string, std::string>> conflictDetails;

	std::function<bool(const std::string&)> DFS = [&](const std::string& amodule) {
		
		visited.insert(amodule);
		recursionStack.push_back(amodule);

		if (graph.contains(amodule)) {
			for (const auto& dependency : graph.at(amodule)) {

				if (std::find(recursionStack.begin(), recursionStack.end(), dependency) != recursionStack.end()) {

					for(auto i : std::views::iota(0ull, recursionStack.size() - 1))
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

		return std::make_optional(ss.str());
	}

	return std::nullopt;
}

std::string CProjectModules::DependencyGraphToString() noexcept
{
	std::stringstream ss;

	for (const auto& [sourceFile, dependencies] : m_oDependencyGraph) {
		ss << sourceFile << " imports:\n";

		if (dependencies.empty()) {
			ss << " - None\n"; 
		}
		else {
			for (const auto& dependency : dependencies) {
				ss << " - " << dependency << '\n';
			}
		}
	}

	return ss.str();
}

