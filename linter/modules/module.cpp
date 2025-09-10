#include "module.hpp"

#include "varjus_api/internal/structure.hpp"
#include "runtime/modules/rtmodule.hpp"
#include "exports/export.hpp"

#include <ranges>
#include <sstream>
#include <functional>
#include <linter/error.hpp>

CModule::CModule() = default;
CModule::CModule(const VarjusString& filePath) : m_oContext(filePath) {}
CModule::~CModule() = default;


void CModule::AddFunction(__RuntimeFunction&& func) {
	m_oFunctions.emplace_back(std::move(func)); 
}
CRuntimeFunctionBase* CModule::FindFunction(const VarjusString& v) const
{
	const auto it = std::ranges::find(m_oFunctions, v, [](const __RuntimeFunction& v) { return v->GetName(); });
	return it != m_oFunctions.end() ? it->get() : nullptr;
}
size_t CModule::GetFunctionCount() const noexcept {
	return m_oFunctions.size();
}

void CModule::AddGlobalInstructions(VectorOf<__RuntimeBlock>&& insns) {
	m_oGlobalScopeInstructions = std::move(insns);
}
void CModule::SetGlobalVariableCount(std::size_t v) { 
	m_uNumGlobalVariables = v; 
}

std::unique_ptr<Varjus::CRuntimeModule> CModule::ToRuntimeModule(){
	return std::make_unique<Varjus::CRuntimeModule>(*this);
}
void CModule::AddExport(const VarjusString& name, UniqueExportedSymbol&& symbol) {
	m_oModuleExports[name] = std::move(symbol);
}
CExportedSymbol* CModule::GetExport(const VarjusString& name) const {
	if (!m_oModuleExports.contains(name))
		return nullptr;

	return m_oModuleExports.at(name).get();
}

/***********************************************************************
 > 
***********************************************************************/

CProjectModules::CProjectModules() = default;
CProjectModules::~CProjectModules() = default;

CModule* CProjectModules::CreateNewModule(const VarjusString& filePath)
{
	auto index = m_oAllModules.size();
	auto& thisModule = m_oAllModules.emplace_back(std::make_unique<CModule>(filePath));
	thisModule->SetIndex(index);
	m_oCachedModules[filePath] = thisModule.get();
	return thisModule.get();
}
CModule* CProjectModules::FindCachedModule(const VarjusString& filePath)
{
	if (!m_oCachedModules.contains(filePath))
		return nullptr;

	return m_oCachedModules.at(filePath);
}
RuntimeModules CProjectModules::ToRuntimeModules()
{
	RuntimeModules modules;
	for (auto& tMod : m_oAllModules) {
		modules.emplace_back(tMod->ToRuntimeModule());
	}

	m_oAllModules.clear();
	return modules;
}

std::optional<VarjusString> CProjectModules::CheckCircularDependencies(const VarjusString& src, const DependencyGraph& graph)
{
	if (graph.empty())
		return std::nullopt;

	std::unordered_set<VarjusString> visited;
	VectorOf<VarjusString> recursionStack;
	VectorOf<std::pair<VarjusString, VarjusString>> conflictDetails;

	std::function<bool(const VarjusString&)> DFS = [&](const VarjusString& amodule) {
		
		visited.insert(amodule);
		recursionStack.push_back(amodule);

		if (graph.contains(amodule)) {
			for (const auto& dependency : graph.at(amodule)) {

				if (std::find(recursionStack.begin(), recursionStack.end(), dependency) != recursionStack.end()) {
					for(auto i : std::views::iota(VarjusUInt(0), recursionStack.size() - VarjusUInt(1)))
						conflictDetails.emplace_back(recursionStack[i], recursionStack[i + VarjusUInt(1)]);
					
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

		VarjusString ss = VSL("circular dependency detected involving the following files:\n");
		for (const auto& [source, target] : conflictDetails)
			ss += Varjus::fmt::format(VSL(" - {} <-> {}\n"), source, target);
		return std::make_optional(ss);
	}

	return std::nullopt;
}

VarjusString CProjectModules::DependencyGraphToString() noexcept
{
	VarjusString ss;

	for (const auto& [sourceFile, dependencies] : m_oDependencyGraph) {
		ss += Varjus::fmt::format(VSL("{} imports:\n"), sourceFile);

		if (dependencies.empty()) {
			ss += VSL(" - None\n"); 
		}
		else {
			for (const auto& dependency : dependencies) {
				ss += Varjus::fmt::format(VSL(" - {}\n"), dependency);
			}
		}
	}

	return ss;
}

