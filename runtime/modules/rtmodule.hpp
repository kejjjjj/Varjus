#pragma once

#include <vector>
#include <memory>

#include "globalDefinitions.hpp"
#include "linter/context.hpp"
#include "runtime/values/types/internal_objects/internal_objects2.hpp"

class CVariable;
class CRuntimeFunction;
class IRuntimeStructure;
class CModule;

using RuntimeFunction = std::unique_ptr<CRuntimeFunction>;
using RuntimeBlock = std::unique_ptr<IRuntimeStructure>;

template<typename T>
using VectorOf = std::vector<T>;

class CRuntimeModule : public CModuleMethods
{
	NONCOPYABLE(CRuntimeModule);

	friend class CProgramRuntime;

public:
	explicit CRuntimeModule(CModule& ctx);
	~CRuntimeModule();

	[[nodiscard]] CRuntimeFunction* GetFunctionByIndex(std::size_t index);
	[[nodiscard]] CVariable* GetGlobalVariableByIndex(std::size_t index);

	void SetupGlobalVariables();
	void EvaluateGlobalExpressions();
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

