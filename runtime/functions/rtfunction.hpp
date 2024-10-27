#pragma once

#include <vector>
#include <memory>

#include "globalDefinitions.hpp"

template <typename T>
using VectorOf = std::vector<T>;

class CVariable;

class CFunction
{
	NONCOPYABLE(CFunction);
	friend class CRuntimeFunction;
public:
	CFunction(VectorOf<std::unique_ptr<CVariable>>&& variables);

	[[nodiscard]] CVariable* GetVariableByIndex(std::size_t index) const;

private:
	VectorOf<std::unique_ptr<CVariable>> m_oStack;
};
