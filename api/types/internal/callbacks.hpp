#pragma once

#include <memory>
#include <unordered_map>
#include <string>
#include <vector>

class IValue;
template<typename T>
using VectorOf = std::vector<T>;
using IValues = VectorOf<IValue*>;

using Function_t = IValue*(*)(struct CRuntimeContext* const, const IValues&);

struct FunctionData_t
{
	Function_t callable;
	std::size_t numArgs;
};

using BuiltInFunction_t = std::unordered_map<std::string, FunctionData_t>;


class CBuiltInFunctions {
public:
	static inline void AddNewGlobalCallable(const std::string& name, const Function_t& callable, std::size_t numArgs) {
		m_arrData[name] = { callable, numArgs };
	}

	[[nodiscard]] static inline auto& Iterator() noexcept { return m_arrData; }
	[[nodiscard]] static inline auto& At(const std::string& s) { return m_arrData.at(s); }
	[[nodiscard]] static inline auto Contains(const std::string& s) { return m_arrData.contains(s); }
	static void Reset() noexcept { m_arrData.clear(); }

private:
	static BuiltInFunction_t m_arrData;
};
