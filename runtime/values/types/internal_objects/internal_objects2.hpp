#pragma once

#include <vector>
#include <memory>
#include <functional>

#include "runtime/values/types/internal/methods.hpp"

class CModuleMethods
{
protected:
	[[nodiscard]] std::vector<class CBuiltInObject*> SetupBuiltInMethods(const std::vector<std::function<BuiltInMethod_t()>>& ctors);

};

#include "runtime/values/types/internal_objects/console.hpp"
#include "runtime/values/types/internal_objects/math.hpp"

class CBuiltInObjects {
public:
	static void AddNewGlobalObject(const std::string& name, const std::function<BuiltInMethod_t()>& createMethods);

	[[nodiscard]] static auto& Iterator() noexcept { return m_arrMethods; }

private:
	static std::vector<std::pair<std::string, std::function<BuiltInMethod_t()>>> m_arrMethods;
};
