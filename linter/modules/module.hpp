#pragma once

#include "linter/expressions/definitions.hpp"
#include "linter/context.hpp"

#include "runtime/structure.hpp"

#include <unordered_set>

class IRuntimeStructure;
class CRuntimeFunction;
class CRuntimeModule;
class CExportedSymbol;

using RuntimeBlock = std::unique_ptr<IRuntimeStructure>;
using RuntimeFunction = std::unique_ptr<CRuntimeFunction>;
using UniqueExportedSymbol = std::unique_ptr<CExportedSymbol>;

using RuntimeModules = VectorOf<std::unique_ptr<CRuntimeModule>>;
using ModuleExports = std::unordered_map<std::string, UniqueExportedSymbol>;
using DependencyGraph = std::unordered_map<std::string, VectorOf<std::string>>;
class CModule
{
	NONCOPYABLE(CModule);

	friend class CBufferLinter;
	friend class CRuntimeModule;

public:
	CModule();
	CModule(const std::string& filePath);
	~CModule();

	[[nodiscard]] CRuntimeFunction* FindFunction(const std::string& v) const;
	[[nodiscard]] size_t GetFunctionCount() const noexcept;

	void AddFunction(RuntimeFunction&& func);
	void AddGlobalInstructions(VectorOf<RuntimeBlock>&& insns);
	void SetGlobalVariableCount(std::size_t v);

	[[nodiscard]] std::unique_ptr<CRuntimeModule> ToRuntimeModule();
	[[nodiscard]] CFileContext* GetContext() noexcept { return &m_oContext; }

	[[nodiscard]] constexpr auto GetIndex() const noexcept { return m_uIndex; }
	[[nodiscard]] constexpr auto& GetFilePath() const noexcept { return m_oContext.m_sFilePath; }

	void AddExport(const std::string& name, UniqueExportedSymbol&& symbol);
	[[nodiscard]] CExportedSymbol* GetExport(const std::string& name) const;
	[[nodiscard]] constexpr auto& GetExports() const noexcept { return m_oModuleExports; }
private:

	std::size_t m_uNumGlobalVariables{};
	VectorOf<RuntimeBlock> m_oGlobalScopeInstructions;
	VectorOf<RuntimeFunction> m_oFunctions;
	ModuleExports m_oModuleExports;

	CFileContext m_oContext;
	std::size_t m_uIndex{};
public:

	static void ResetEverythingStatic();

	[[nodiscard]] static CModule* CreateNewModule(const std::string& filePath);
	[[nodiscard]] static CModule* FindCachedModule(const std::string& filePath);
	[[nodiscard]] static RuntimeModules ToRuntimeModules();

	[[nodiscard]] static std::string DependencyGraphToString() noexcept;
	[[nodiscard]] static Success CheckCircularDependencies(const std::string& src, const DependencyGraph& graph);

	static DependencyGraph m_oDependencyGraph;
	static std::unordered_set<std::string> m_oVisitedModules;

private:


	static VectorOf<std::unique_ptr<CModule>> m_oAllModules;
	static std::unordered_map<std::string, CModule*> m_oCachedModules;

};
