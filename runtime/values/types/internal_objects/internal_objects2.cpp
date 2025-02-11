#include "internal_objects2.hpp"
#include "internal_objects.hpp"
#include "runtime/runtime.hpp"
#include "runtime/structure.hpp"

std::vector<CBuiltInObject*> CModuleMethods::SetupBuiltInMethods(const std::vector<std::function<BuiltInMethod_t()>>& ctors)
{



	std::vector<CBuiltInObject*> values;
	for (auto& ctor : ctors) {
		values.push_back(CBuiltInObject::Construct(ctor()));
	}

	return values;
}

std::vector<std::pair<std::string, std::function<BuiltInMethod_t()>>> CBuiltInObjects::m_arrMethods;
void CBuiltInObjects::AddNewGlobalObject(const std::string& name, const std::function<BuiltInMethod_t()>& createMethods)
{
	m_arrMethods.push_back({ name, createMethods });
}


