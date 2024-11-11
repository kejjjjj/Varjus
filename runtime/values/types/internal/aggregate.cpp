#include "aggregate.hpp"

#include "runtime/runtime.hpp"
#include "runtime/structure.hpp"
#include "runtime/values/types/simple.hpp"
#include "runtime/exceptions/exception.hpp"

#include <cassert>
#include <stdexcept>

void CAggregate::Setup(const std::vector<ElementIndex>& elements)
{
	for (auto& l : elements) {
		if (l == ARRAY_LENGTH)
			m_oIndexLookup[l] = CProgramRuntime::AcquireNewValue<CIntValue>(0);
		else
			m_oIndexLookup[l] = CProgramRuntime::AcquireNewValue<IValue>();
	}

	assert(elements.size());
}
IValue* CAggregate::ElementLookup(GlobalMemberIndex index) const
{

	try {
		return m_oIndexLookup.at(index);
	}
	catch ([[maybe_unused]]std::out_of_range& ex) {
		throw CRuntimeError("this aggregate type doesn't contain this member");
	}

}
