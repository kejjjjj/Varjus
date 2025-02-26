#pragma once

#include "linter/expressions/definitions.hpp"
#include "linter/context.hpp"
#include "linter/error.hpp"

#include "api/internal/structure.hpp"

#include <unordered_set>
#include <optional>
#include <string>

class IRuntimeStructure;
class CRuntimeFunctionBase;
class CRuntimeModule;
class CExportedSymbol;
class CModule;

using RuntimeBlock = std::unique_ptr<IRuntimeStructure>;
using RuntimeFunction = std::unique_ptr<CRuntimeFunctionBase>;
using UniqueExportedSymbol = std::unique_ptr<CExportedSymbol>;

using RuntimeModules = VectorOf<std::unique_ptr<CRuntimeModule>>;
using ModuleExports = std::unordered_map<std::string, UniqueExportedSymbol>;
using DependencyGraph = std::unordered_map<std::string, VectorOf<std::string>>;

class CProjectModules
{
	NONCOPYABLE(CProjectModules);
public:
	CProjectModules();
	~CProjectModules();

	[[nodiscard]] CModule* CreateNewModule(const std::string& filePath);
	[[nodiscard]] CModule* FindCachedModule(const std::string& filePath);
	[[nodiscard]] RuntimeModules ToRuntimeModules();

	[[nodiscard]] std::string DependencyGraphToString() noexcept;
	[[nodiscard]] std::optional<std::string> CheckCircularDependencies(const std::string& src, const DependencyGraph& graph);

	DependencyGraph m_oDependencyGraph;
	std::unordered_set<std::string> m_oVisitedModules;

private:
	VectorOf<std::unique_ptr<CModule>> m_oAllModules;
	std::unordered_map<std::string, CModule*> m_oCachedModules;
};

class CModule final
{
	NONCOPYABLE(CModule);

	friend class CBufferLinter;
	friend class CRuntimeModule;

public:

	CModule();
	CModule(const std::string& filePath);
	~CModule();

	[[nodiscard]] CRuntimeFunctionBase* FindFunction(const std::string& v) const;
	[[nodiscard]] size_t GetFunctionCount() const noexcept;

	void AddFunction(RuntimeFunction&& func);
	void AddGlobalInstructions(VectorOf<RuntimeBlock>&& insns);
	void SetGlobalVariableCount(std::size_t v);

	[[nodiscard]] std::unique_ptr<CRuntimeModule> ToRuntimeModule();
	[[nodiscard]] CFileContext* GetContext() noexcept { return &m_oContext; }

	[[nodiscard]] constexpr auto GetIndex() const noexcept { return m_uIndex; }
	constexpr void SetIndex(std::size_t i) noexcept { m_uIndex = i; }

	[[nodiscard]] constexpr auto& GetFilePath() const noexcept { return m_oContext.m_sFilePath; }

	void AddExport(const std::string& name, UniqueExportedSymbol&& symbol);
	[[nodiscard]] CExportedSymbol* GetExport(const std::string& name) const;
	[[nodiscard]] constexpr auto& GetExports() const noexcept { return m_oModuleExports; }

	[[nodiscard]] inline auto PushError(const std::string& err) noexcept { m_oErrors.PushError(GetFilePath(), err); }
	[[nodiscard]] inline auto PushError(const std::string& err, const CodePosition& pos) noexcept {
		m_oErrors.PushError(GetFilePath(), err, pos); 
	}

private:

	std::size_t m_uNumGlobalVariables{};
	VectorOf<RuntimeBlock> m_oGlobalScopeInstructions;
	VectorOf<RuntimeFunction> m_oFunctions;
	ModuleExports m_oModuleExports;
	CLinterErrors m_oErrors; //file specific errors

	CFileContext m_oContext;
	std::size_t m_uIndex{};
};
