#pragma once

#include <memory>
#include <unordered_map>

#include <vector>

#include "varjus_api/internal/globalDefinitions.hpp"


class IValue;
template<typename T>
using VectorOf = std::vector<T>;
using IValues = VectorOf<IValue*>;

#define VARJUS_FORWARD_DECLARE_CALLBACK(name)\
IValue* name(struct CRuntimeContext* const ctx, const IValues& args);
#define VARJUS_DEFINE_CALLBACK(name, ctx, args)\
IValue* name([[maybe_unused]] struct CRuntimeContext* const ctx, [[maybe_unused]] const IValues& args)

using Function_t = IValue*(*)(struct CRuntimeContext* const, const IValues&);

struct FunctionData_t
{
	Function_t callable;
	std::size_t numArgs;
};

using BuiltInFunction_t = std::unordered_map<VarjusString, FunctionData_t>;


class CBuiltInFunctions {
public:
	inline void AddNewGlobalCallable(const VarjusString& name, const Function_t& callable, std::size_t numArgs) {
		m_arrData[name] = { callable, numArgs };
	}

	[[nodiscard]] inline auto& Iterator() noexcept { return m_arrData; }
	[[nodiscard]] inline auto& At(const VarjusString& s) { return m_arrData.at(s); }
	[[nodiscard]] inline auto Contains(const VarjusString& s) { return m_arrData.contains(s); }
	inline void Reset() noexcept { m_arrData.clear(); }

private:
	BuiltInFunction_t m_arrData;
};
