#include "internal_methods.hpp"
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


