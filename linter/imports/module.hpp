#pragma once

#include "linter/expressions/definitions.hpp"
#include "linter/context.hpp"

#include "runtime/structure.hpp"

class IRuntimeStructure;
class CRuntimeFunction;
class CRuntimeModule;

using RuntimeBlock = std::unique_ptr<IRuntimeStructure>;
using RuntimeFunction = std::unique_ptr<CRuntimeFunction>;

using RuntimeModules = VectorOf<std::unique_ptr<CRuntimeModule>>;
using ModuleExports = std::unordered_map<std::string, RuntimeBlock>;

class CModule
{
	NONCOPYABLE(CModule);

	friend class CFileLinter;
	friend class CRuntimeModule;

public:
	CModule() = default;
	CModule(const std::string& filePath) : m_oContext(filePath) {}
	~CModule();

	[[nodiscard]] CRuntimeFunction* FindFunction(const std::string& v) const;
	[[nodiscard]] size_t GetFunctionCount() const noexcept;

	void AddFunction(RuntimeFunction&& func);

	void AddGlobalInstructions(VectorOf<RuntimeBlock>&& insns);
	void SetGlobalVariableCount(std::size_t v);

	[[nodiscard]] std::unique_ptr<CRuntimeModule> ToRuntimeModule();
	[[nodiscard]] CFileContext* GetContext() noexcept { return &m_oContext; }

	[[nodiscard]] constexpr auto GetIndex() const noexcept { return m_uIndex; }
private:

	std::size_t m_uNumGlobalVariables{};
	VectorOf<RuntimeBlock> m_oGlobalScopeInstructions;
	VectorOf<RuntimeFunction> m_oFunctions;
	ModuleExports m_oModuleExports;

	VectorOf<RuntimeBlock> m_oExports;

	CFileContext m_oContext;
	std::size_t m_uIndex{};
public:

	static CModule* CreateNewModule(const std::string& filePath);
	static CModule* FindCachedModule(const std::string& filePath);
	static RuntimeModules ToRuntimeModules();

	static VectorOf<std::unique_ptr<CModule>> m_oAllModules;
	static std::unordered_map<std::string, CModule*> m_oCachedModules;
};
