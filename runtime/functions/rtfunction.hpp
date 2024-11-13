#pragma once

#include <vector>
#include <memory>

#include "globalDefinitions.hpp"

template <typename T>
using VectorOf = std::vector<T>;

class CVariable;
class IValue;

class CFunction
{
	NONCOPYABLE(CFunction);
	friend class CRuntimeFunction;
public:
	CFunction(VectorOf<IValue*>& args, VectorOf<CVariable*>&& variables);

	[[nodiscard]] CVariable* GetVariableByIndex(std::size_t index) const;

private:
	VectorOf<CVariable*> m_oStack;
};
