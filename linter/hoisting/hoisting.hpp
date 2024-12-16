#pragma once

#include <unordered_map>
#include <string>
#include <cassert>

#include "linter/functions/memory/manager_function.hpp"

using HoistedFunction = std::unordered_map<std::string, std::unique_ptr<CLinterFunction>>;

class CHoister
{
public:

	void DeclareFunction(const std::string& name, CLinterFunction& f) {
		m_oHoistedFunctions[name] = std::make_unique<CLinterFunction>(std::move(f));
	}
	[[nodiscard]] bool ContainsFunction(const std::string& name) {
		return m_oHoistedFunctions.find(name) != m_oHoistedFunctions.end();
	}
	[[nodiscard]] inline auto GetFunctionByName(const std::string& name) {
		assert(ContainsFunction(name));
		return m_oHoistedFunctions.find(name)->second.get();
	}

private:
	HoistedFunction m_oHoistedFunctions;
};
