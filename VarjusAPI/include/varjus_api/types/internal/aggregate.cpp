#include "aggregate.hpp"

#include "varjus_api/types/default.hpp"

#include "linter/expressions/ast.hpp"
#include "linter/context.hpp"

#include "varjus_api/internal/runtime.hpp"
#include "varjus_api/internal/structure.hpp"
#include "varjus_api/internal/variables.hpp"
#include "runtime/exceptions/exception.hpp"
#include "runtime/modules/rtmodule.hpp"

#include <cassert>
#include <stdexcept>

void CAggregate::Setup(CProgramRuntime* const runtime, const std::vector<ElementIndex>& elements){
	m_pAllocator = runtime;
	for (auto& l : elements) {
		AddAttribute(l);
	}
}
CVariable* CAggregate::AddAttribute(ElementIndex elem)
{
	assert(m_pAllocator);
	return m_oIndexLookup[elem] = CVariable::Construct(m_pAllocator, IValue::Construct(m_pAllocator));
}
void CAggregate::AddAttribute(ElementIndex elem, IValue* value){
	return AddAttribute(elem)->SetValue(value);
}
bool CAggregate::RemoveAttribute(ElementIndex elem)
{
	if (!m_oIndexLookup.contains(elem))
		return false;

	auto var = m_oIndexLookup.at(elem);
	var->Release();
	m_oIndexLookup.erase(elem);
	return true;
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
	assert(m_pAllMembers);
	if (!m_oIndexLookup.contains(index)) {
		throw CRuntimeError(m_pAllocator, fmt::format(VSL("this aggregate doesn't have the attribute \"{}\""),
			m_pAllMembers->At(index)
		));
	}
	return m_oIndexLookup.at(index)->GetValue();
}
bool CAggregate::Contains(const VarjusString& item) const {
	
	assert(m_pAllMembers);

	if (!m_pAllMembers->Contains(item))
		return false;

	const auto key = m_pAllMembers->At(item);

	return m_oIndexLookup.contains(key);
}
IValue* CAggregate::Get(const VarjusString& item) const
{
	assert(m_pAllocator->GetInformation());
	auto& members = m_pAllocator->GetInformation()->m_oAllMembers;

	if (!members.Contains(item))
		return nullptr;

	const auto key = members.At(item);

	if (!m_oIndexLookup.contains(key))
		return nullptr;

	return m_oIndexLookup.at(key)->GetValue();
}
#ifdef RUNNING_TESTS
IValue* CAggregate::ElementLookupNoExcept(GlobalMemberIndex index) const noexcept {

	if (!m_oIndexLookup.contains(index))
		return nullptr;

	return m_oIndexLookup.at(index)->GetValue();
}
#endif

