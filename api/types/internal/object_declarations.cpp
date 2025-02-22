#include "object_declarations.hpp"

#include "runtime/structure.hpp"
#include "linter/context.hpp"

BuiltInMethod_t::~BuiltInMethod_t() {
	static_assert(sizeof(CBuiltInRuntimeMethod) > 0, "woops!");
}

void BuiltInMethod_t::AddMethod(const std::string& name, Method_t method, std::size_t numArgs)
{
	if (CFileContext::m_oAllMembers.Contains(name)) {
		const auto id = CFileContext::m_oAllMembers.At(name); 
		if (!contains(id))
			(*this)[id] = std::make_unique<CBuiltInRuntimeMethod>(method, numArgs); 
	}
}

void BuiltInProperty_t::AddProperty(const std::string& name, Property_t property)
{
	if (CFileContext::m_oAllMembers.Contains(name)) {
		const auto id = CFileContext::m_oAllMembers.At(name);
		if (!contains(id))
			(*this)[id] = property;
	}
}
