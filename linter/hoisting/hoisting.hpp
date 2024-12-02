#pragma once

#include <unordered_map>
#include <string>
#include <cassert>

using HoistedFunction = std::unordered_map<std::string, std::size_t>;

class CHoister
{
public:

	void DeclareFunction(const std::string& name) {
		m_oHoistedFunctions[name] = m_oHoistedFunctions.size();
	}
	[[nodiscard]] bool ContainsFunction(const std::string& name) {
		return m_oHoistedFunctions.find(name) != m_oHoistedFunctions.end();
	}
	[[nodiscard]] std::size_t GetFunctionIndexByName(const std::string& name) {
		assert(ContainsFunction(name));
		return m_oHoistedFunctions.find(name)->second;
	}

private:
	HoistedFunction m_oHoistedFunctions;
};
