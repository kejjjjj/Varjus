#pragma once

#include <vector>
#include <memory>

#include "varjus_api/internal/globalDefinitions.hpp"
#include "linter/context.hpp"

class CVariable;
class CRuntimeFunctionBase;
class IRuntimeStructure;
class CModule;
class CProgramRuntime;

using RuntimeFunction = std::unique_ptr<CRuntimeFunctionBase>;
using RuntimeBlock = std::unique_ptr<IRuntimeStructure>;

template<typename T>
using VectorOf = std::vector<T>;

class CRuntimeModule
{
	NONCOPYABLE(CRuntimeModule);

	friend class CProgramRuntime;

public:
	explicit CRuntimeModule(CModule& ctx);
	~CRuntimeModule();

	[[nodiscard]] CRuntimeFunctionBase* GetFunctionByIndex(std::size_t index);
	[[nodiscard]] CVariable* GetGlobalVariableByIndex(std::size_t index);

	void SetupGlobalVariables(CProgramRuntime* const runtime);
	void EvaluateGlobalExpressions(CProgramRuntime* const runtime);
	void FreeGlobalVariables();

	[[nodiscard]] CFileContext* GetContext() noexcept { return &m_oContext; }
	[[nodiscard]] constexpr auto GetIndex() const noexcept { return m_uModuleIndex; }
private:
	VectorOf<RuntimeBlock> m_oGlobalScopeInstructions;
	std::size_t m_uNumGlobalVariables{};

	VectorOf<RuntimeFunction> m_oFunctions;
	VectorOf<CVariable*> m_oGlobalVariables;

	CFileContext m_oContext;
	std::size_t m_uModuleIndex{};


};

