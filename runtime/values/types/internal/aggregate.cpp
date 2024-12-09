#include "aggregate.hpp"

#include "linter/expressions/ast.hpp"
#include "linter/context.hpp"

#include "runtime/runtime.hpp"
#include "runtime/structure.hpp"
#include "runtime/variables.hpp"
#include "runtime/values/types/simple.hpp"
#include "runtime/exceptions/exception.hpp"


#include <cassert>
#include <stdexcept>

void CAggregate::Setup(const std::vector<ElementIndex>& elements){
	for (auto& l : elements) {
		AddAttribute(l);
	}
}
CVariable* CAggregate::AddAttribute(ElementIndex elem)
{
	auto& var = m_oIndexLookup[elem] = CProgramRuntime::AcquireNewVariable();
	var->SetValue(CProgramRuntime::AcquireNewValue<IValue>());
	return var;
}
void CAggregate::AddAttribute(ElementIndex elem, IValue* value){
	return AddAttribute(elem)->SetValue(value);
}
void CAggregate::Release()
{

	for (auto it = m_oIndexLookup.begin(); it != m_oIndexLookup.end(); ) {

		if (!it->second->GetValue() || it->second->Release()) {
			it = m_oIndexLookup.erase(it);
		} else {
			++it;
		}
	}
}
IValue* CAggregate::ElementLookup(GlobalMemberIndex index) const
{

	try {
		return m_oIndexLookup.at(index)->GetValue();
	}
	catch ([[maybe_unused]]std::out_of_range& ex) {

		throw CRuntimeError(std::format("this aggregate doesn't have the attribute \"{}\"",
			CProgramRuntime::GetContext()->m_oAllMembers.At(index)
		));
	}

}

/***********************************************************************
 > 
***********************************************************************/

CVariable* CArrayAggregate::AddAttribute(ElementIndex elem)
{
	auto& var = m_oIndexLookup[elem] = CProgramRuntime::AcquireNewVariable();

	if (elem == LENGTH_PROPERTY) {
		var->SetValue(CProgramRuntime::AcquireNewValue<CIntValue>(0));
		var->GetValue()->MakeImmutable();
	} else {
		var->SetValue(CProgramRuntime::AcquireNewValue<IValue>());
	}

	return var;
}


