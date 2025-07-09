#pragma once

#include <vector>
#include <memory>
#include <unordered_map>

#include "varjus_api/types/internal/references.hpp"
#include "varjus_api/internal/globalDefinitions.hpp"

template <typename T>
using VectorOf = std::vector<T>;

class CVariable;
class IValue;

namespace Varjus {
	class CProgramRuntime;
	class CRuntimeModule;
}

using __VariableCaptures = std::unordered_map<CCrossModuleReference, CVariable*, CCrossModuleReferenceHasher>;

class CFunction
{
	VARJUS_NONCOPYABLE(CFunction);
	friend class CRuntimeFunction;
public:
	CFunction(Varjus::CProgramRuntime* const ctx, VectorOf<IValue*>& args, const __VariableCaptures& captures, const CRuntimeFunction& func);

	[[nodiscard]] CVariable* GetVariableByRef(const CCrossModuleReference& ref) const;


private:
	std::unordered_map<CCrossModuleReference, CVariable*, CCrossModuleReferenceHasher> m_oStack;
};
