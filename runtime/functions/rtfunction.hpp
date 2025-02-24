#pragma once

#include <vector>
#include <memory>
#include <unordered_map>

#include "api/types/internal/references.hpp"
#include "api/internal/globalDefinitions.hpp"

template <typename T>
using VectorOf = std::vector<T>;

class CVariable;
class IValue;
class CRuntimeModule;
class CProgramRuntime;

template<typename A, typename B>
using KeyValue = std::pair<A, B>;
using VariableIndex = std::size_t;

using VariableCaptures = std::unordered_map<CCrossModuleReference, CVariable*, CCrossModuleReferenceHasher>;

class CFunction
{
	NONCOPYABLE(CFunction);
	friend class CRuntimeFunction;
public:
	CFunction(CProgramRuntime* const ctx, VectorOf<IValue*>& args, const VariableCaptures& captures, const CRuntimeFunction& func);

	[[nodiscard]] CVariable* GetVariableByRef(const CCrossModuleReference& ref) const;


private:
	std::unordered_map<CCrossModuleReference, CVariable*, CCrossModuleReferenceHasher> m_oStack;
};
