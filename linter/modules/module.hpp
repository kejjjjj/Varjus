#pragma once

#include "linter/expressions/definitions.hpp"
#include "linter/context.hpp"
#include "linter/error.hpp"

#include "varjus_api/internal/structure.hpp"

#include <unordered_set>
#include <optional>


class IRuntimeStructure;
class CRuntimeFunctionBase;
class CExportedSymbol;
class CModule;

namespace Varjus {
	class CRuntimeModule;
}

using __RuntimeBlock = std::unique_ptr<IRuntimeStructure>;
using __RuntimeFunction = std::unique_ptr<CRuntimeFunctionBase>;
using UniqueExportedSymbol = std::unique_ptr<CExportedSymbol>;

using RuntimeModules = VectorOf<std::unique_ptr<Varjus::CRuntimeModule>>;
using ModuleExports = std::unordered_map<VarjusString, UniqueExportedSymbol>;
using DependencyGraph = std::unordered_map<VarjusString, VectorOf<VarjusString>>;

class CProjectModules
{
	VARJUS_NONCOPYABLE(CProjectModules);
public:
	CProjectModules();
	~CProjectModules();

	[[nodiscard]] CModule* CreateNewModule(const VarjusString& filePath);
	[[nodiscard]] CModule* FindCachedModule(const VarjusString& filePath);
	[[nodiscard]] RuntimeModules ToRuntimeModules();

	[[nodiscard]] VarjusString DependencyGraphToString() noexcept;
	[[nodiscard]] std::optional<VarjusString> CheckCircularDependencies(const VarjusString& src, const DependencyGraph& graph);

	DependencyGraph m_oDependencyGraph;
	std::unordered_set<VarjusString> m_oVisitedModules;

private:
	VectorOf<std::unique_ptr<CModule>> m_oAllModules;
	std::unordered_map<VarjusString, CModule*> m_oCachedModules;
};

class CModule final
{
	VARJUS_NONCOPYABLE(CModule);

	friend class CBufferLinter;
	friend class Varjus::CRuntimeModule;

public:

	CModule();
	CModule(const VarjusString& filePath);
	~CModule();

	[[nodiscard]] CRuntimeFunctionBase* FindFunction(const VarjusString& v) const;
	[[nodiscard]] size_t GetFunctionCount() const noexcept;

	void AddFunction(__RuntimeFunction&& func);
	void AddGlobalInstructions(VectorOf<__RuntimeBlock>&& insns);
	void SetGlobalVariableCount(std::size_t v);

	[[nodiscard]] std::unique_ptr<Varjus::CRuntimeModule> ToRuntimeModule();
	[[nodiscard]] CFileContext* GetContext() noexcept { return &m_oContext; }

	[[nodiscard]] constexpr auto GetIndex() const noexcept { return m_uIndex; }
	constexpr void SetIndex(std::size_t i) noexcept { m_uIndex = i; }

	[[nodiscard]] constexpr auto& GetFilePath() const noexcept { return m_oContext.m_sFilePath; }

	void AddExport(const VarjusString& name, UniqueExportedSymbol&& symbol);
	[[nodiscard]] CExportedSymbol* GetExport(const VarjusString& name) const;
	[[nodiscard]] constexpr auto& GetExports() const noexcept { return m_oModuleExports; }

	[[nodiscard]] inline auto PushError(const VarjusString& err) { m_oErrors.PushError(GetFilePath(), err); }
	[[nodiscard]] inline auto PushError(const VarjusString& err, const __CodePosition& pos) {
		m_oErrors.PushError(GetFilePath(), err, pos); 
	}

private:

	std::size_t m_uNumGlobalVariables{};
	VectorOf<__RuntimeBlock> m_oGlobalScopeInstructions;
	VectorOf<__RuntimeFunction> m_oFunctions;
	ModuleExports m_oModuleExports;
	CLinterErrors m_oErrors; //file specific errors

	CFileContext m_oContext;
	std::size_t m_uIndex{};
};
