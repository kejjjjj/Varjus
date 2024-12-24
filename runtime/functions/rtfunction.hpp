#pragma once

#include <vector>
#include <memory>
#include <unordered_map>

#include "linter/modules/references.hpp"
#include "globalDefinitions.hpp"

template <typename T>
using VectorOf = std::vector<T>;

class CVariable;
class IValue;
class CRuntimeModule;

template<typename A, typename B>
using KeyValue = std::pair<A, B>;
using VariableIndex = std::size_t;

using IndexToVariable = KeyValue<VariableIndex, CVariable*>;
using VariableCaptures = std::unordered_map<CCrossModuleReference, CVariable*, CCrossModuleReferenceHasher>;

class CFunction
{
	NONCOPYABLE(CFunction);
	friend class CRuntimeFunction;
public:
	CFunction(VectorOf<IValue*>& args,
		const VariableCaptures& captures, const CRuntimeFunction& func);

	[[nodiscard]] CVariable* GetVariableByRef(const CCrossModuleReference& ref) const;


private:
	std::unordered_map<CCrossModuleReference, CVariable*, CCrossModuleReferenceHasher> m_oStack;
};
