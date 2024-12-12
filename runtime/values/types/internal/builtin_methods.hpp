#pragma once

#include "runtime/values/types/simple.hpp"
#include "runtime/exceptions/exception.hpp"

#include "linter/context.hpp"

#include <unordered_map>
#include <string>
#include <stdexcept>
#include <functional>
#include <format>
#include <type_traits>
class CFunction;
using ElementIndex = std::size_t;


namespace BuiltInMethods
{
	void Setup(const CProgramContext* context);
}

constexpr auto UNCHECKED_PARAMETER_COUNT = std::numeric_limits<std::size_t>::max();

template<typename Type>
struct CBuiltInMethod
{
	std::size_t m_uNumArguments;
	IValue* (Type::* memFn)(CFunction* const, const IValues&);
};

template<class Type>
class CBuiltInMethods
{
public:
	using MethodType = CBuiltInMethod<Type>;
	using InputType = std::unordered_map<std::string, MethodType>;
	using LookupType = std::unordered_map<ElementIndex, const MethodType*>;

	friend void BuiltInMethods::Setup(const CProgramContext* context);

private:
	static LookupType m_oMethodLookup;

public:

	static inline void Initialize(const CProgramContext* context, const InputType& methods) {
		for (auto& [name, id] : context->m_oAllMembers.StringToIntIterator()) {
			if (methods.contains(name)) {
				const auto& [_, method] = *methods.find(name);
				m_oMethodLookup[id] = &method;
			}
		}
	}

	[[nodiscard]] static inline const MethodType* LookupMethod(ElementIndex index) {

		if (m_oMethodLookup.contains(index))
			return m_oMethodLookup[index];

		return nullptr;
	}

	[[nodiscard]] static inline IValue* CallMethod(CFunction* const thisFunction,
		Type* _this, const IValues& args, const MethodType* method) {

		if (method->m_uNumArguments != UNCHECKED_PARAMETER_COUNT && method->m_uNumArguments != args.size())
			throw CRuntimeError(std::format("the method expected {} arguments instead of {}", method->m_uNumArguments, args.size()));

		auto returnVal = std::mem_fn(method->memFn)(_this, thisFunction, args);

		for (auto& val : args)
			val->Release();

		return returnVal;
	}

	[[nodiscard]] static inline auto& GetIterator() noexcept { return m_oMethodLookup; }

private:
};

