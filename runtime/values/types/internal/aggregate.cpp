#include "aggregate.hpp"

#include "runtime/runtime.hpp"
#include "runtime/structure.hpp"
#include "runtime/variables.hpp"
#include "runtime/values/types/simple.hpp"
#include "runtime/exceptions/exception.hpp"

#include <cassert>
#include <stdexcept>

void CAggregate::Setup(const std::vector<ElementIndex>& elements)
{

	for (auto& l : elements) {
		
		const auto& var = (m_oIndexLookup[l] = std::move(CProgramRuntime::m_oVariablePool.Acquire()));

		if (l == ARRAY_LENGTH) {			
			var->SetValue(CProgramRuntime::AcquireNewValue<CIntValue>(0));
			var->GetValue()->MakeImmutable();
		}
		else {
			var->SetValue(CProgramRuntime::AcquireNewValue<IValue>());
		}
	}

	assert(elements.size());
}
void CAggregate::Release()
{
	for (auto& [idx, v] : m_oIndexLookup) {
		auto& value = v->GetValue();
		value->Release();
		value = nullptr;
		CProgramRuntime::m_oVariablePool.Release(std::move(v));
	}

}
IValue* CAggregate::ElementLookup(GlobalMemberIndex index) const
{

	try {
		return m_oIndexLookup.at(index)->GetValue();
	}
	catch ([[maybe_unused]]std::out_of_range& ex) {
		throw CRuntimeError("this aggregate type doesn't contain this member");
	}

}
