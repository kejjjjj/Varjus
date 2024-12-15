#pragma once

#include <unordered_map>
#include <string>
#include <cassert>

#include "linter/functions/memory/manager_function.hpp"

using HoistedFunction = std::unordered_map<std::string, CLinterFunction>;

class CHoister
{
public:

	void DeclareFunction(const std::string& name, CLinterFunction& f) {
		m_oHoistedFunctions[name] = std::move(f);
	}
	[[nodiscard]] bool ContainsFunction(const std::string& name) {
		return m_oHoistedFunctions.find(name) != m_oHoistedFunctions.end();
	}
	[[nodiscard]] inline auto& GetFunctionByName(const std::string& name) {
		assert(ContainsFunction(name));
		return m_oHoistedFunctions.find(name)->second;
	}
	[[nodiscard]] std::size_t GetFunctionIndexByName(const std::string& name) {
		assert(ContainsFunction(name));
		return m_oHoistedFunctions.find(name)->second.m_uIndex;
	}

private:
	HoistedFunction m_oHoistedFunctions;
};
