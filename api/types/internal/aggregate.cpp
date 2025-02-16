#include "aggregate.hpp"

#include "api/types/default.hpp"

#include "linter/expressions/ast.hpp"
#include "linter/context.hpp"

#include "runtime/runtime.hpp"
#include "runtime/structure.hpp"
#include "runtime/variables.hpp"
#include "runtime/exceptions/exception.hpp"
#include "runtime/modules/rtmodule.hpp"

#include <cassert>
#include <stdexcept>

void CAggregate::Setup(std::size_t moduleIndex, const std::vector<ElementIndex>& elements){
	SetModuleIndex(moduleIndex);
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
	if (!m_oIndexLookup.contains(index)) {
		throw CRuntimeError(std::format("this aggregate doesn't have the attribute \"{}\"",
			CFileContext::m_oAllMembers.At(index)
		));
	}
	return m_oIndexLookup.at(index)->GetValue();
}
bool CAggregate::Contains(const std::string& item) const {
	
	if (!CFileContext::m_oAllMembers.Contains(item))
		return false;

	const auto key = CFileContext::m_oAllMembers.At(item);

	return m_oIndexLookup.contains(key);
}
IValue* CAggregate::Get(const std::string& item) const
{
	if (!CFileContext::m_oAllMembers.Contains(item))
		return nullptr;

	const auto key = CFileContext::m_oAllMembers.At(item);

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

