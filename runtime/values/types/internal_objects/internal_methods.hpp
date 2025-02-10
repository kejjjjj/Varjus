#include <vector>
#include <memory>
#include <functional>

#include "runtime/values/types/internal/methods.hpp"

class CModuleMethods
{
protected:
	[[nodiscard]] std::vector<class CBuiltInObject*> SetupBuiltInMethods(const std::vector<std::function<BuiltInMethod_t()>>& ctors);

};

