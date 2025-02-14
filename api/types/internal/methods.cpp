#include "methods.hpp"

#include "runtime/structure.hpp"
#include "linter/context.hpp"

void BuiltInMethod_t::AddMethod(const std::string& name, Method_t method, std::size_t numArgs)
{
	if (CFileContext::m_oAllMembers.Contains(name)) {
		const auto id = CFileContext::m_oAllMembers.At(name); 
		if (!contains(id))
			(*this)[id] = std::make_unique<CBuiltInRuntimeFunction>(method, numArgs); 
	}
}
