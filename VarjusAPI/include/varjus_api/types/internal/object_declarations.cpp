#include "object_declarations.hpp"

#include "varjus_api/internal/structure.hpp"
#include "linter/context.hpp"
#include <cassert>

using namespace Varjus;

void BuiltInMethod_t::AddMethod(const VarjusString& name, Method_t method, std::size_t numArgs)
{
	assert(m_pInfo);
	if (!m_pInfo->m_oAllMembers.Contains(name)) 
		m_pInfo->m_oAllMembers.Insert(name);

	const auto id = m_pInfo->m_oAllMembers.At(name);
	if (!contains(id))
		(*this)[id] = std::make_unique<CBuiltInRuntimeMethod>(method, numArgs); 
}


void BuiltInProperty_t::AddProperty(const VarjusString& name, Property_t property)
{
	assert(m_pInfo);
	if (!m_pInfo->m_oAllMembers.Contains(name)) 
		m_pInfo->m_oAllMembers.Insert(name);

	const auto id = m_pInfo->m_oAllMembers.At(name);
	if (!contains(id))
		(*this)[id] = property;	
}
