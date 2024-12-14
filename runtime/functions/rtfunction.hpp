#pragma once

#include <vector>
#include <memory>
#include <unordered_map>

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
using VariableCaptures = std::unordered_map<VariableIndex, CVariable*>;


class CFunction
{
	NONCOPYABLE(CFunction);
	friend class CRuntimeFunction;
public:
	CFunction(std::size_t ownerModule, VectorOf<IValue*>& args,
		const VariableCaptures& captures, const CRuntimeFunction& func);

	[[nodiscard]] CVariable* GetVariableByIndex(std::size_t index) const;

	constexpr auto GetModuleIndex() const noexcept { return m_uModuleIndex; }

private:
	std::unordered_map<VariableIndex, CVariable*> m_oStack;
	std::size_t m_uModuleIndex{};
};
