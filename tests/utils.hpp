#pragma once


#include "globalDefinitions.hpp"
#include "utils.hpp"
#include "tests.hpp"
#include "runtime/runtime.hpp"
#include "runtime/structure.hpp"
#include "runtime/modules/rtmodule.hpp"
#include "runtime/values/types/types.hpp"

#include <vector>
#include <string>

using ASSERT_BOOL = bool;
using ASSERT_INT = std::int64_t;
using ASSERT_DOUBLE = double;
using ASSERT_STRING = std::string;
using ASSERT_UNDEFINED = void;

template<typename T>
struct AssertObjectValue
{
	EValueType m_type;
	std::size_t m_moduleIndex{};
	std::vector<std::pair<std::string, T>> m_targetValues;
};

template<typename T>
struct AssertArrayValue
{
	EValueType m_type;
	std::vector<T> m_targetValues;
};

template<typename T> 
constexpr auto GetTemplatedValue(IValue* t)
{
	if constexpr (std::is_same_v<ASSERT_BOOL, T>)
		return t->ToBoolean();
	if constexpr (std::is_same_v<ASSERT_INT, T>)
		return t->ToInt();
	else if constexpr (std::is_same_v<ASSERT_DOUBLE, T>)
		return t->ToDouble();
	else if constexpr (std::is_same_v<ASSERT_STRING, T>)
		return t->ToString();
	else
		static_assert(false, "bad type");
}

template<typename T>
void AssertObject(IValue* retVal, const AssertObjectValue<T>& assertValue)
{
	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_object);
	REQUIRE(retVal->IsSharedObject() == true);

	auto object = retVal->ToObject();
	REQUIRE(object != nullptr);
	auto internal = object->Internal();
	REQUIRE(internal != nullptr);
	auto& aggregate = internal->Get();
	auto& items = aggregate.Iterator();

	REQUIRE(items.size() == assertValue.m_targetValues.size());

	auto m = CProgramRuntime::GetModuleByIndex(assertValue.m_moduleIndex);
	auto& allMembers = m->GetContext()->m_oAllMembers;

	for (auto& [name, value] : assertValue.m_targetValues) {
		REQUIRE(allMembers.Contains(name));
		IValue* result = aggregate.ElementLookupNoExcept(allMembers[name]);
		REQUIRE(result->HasOwner() == true);
		REQUIRE(result != nullptr);
		REQUIRE(result->Type() == assertValue.m_type);
		REQUIRE(GetTemplatedValue<T>(result) == value);
	}
}

template<typename T>
void AssertArray(IValue* retVal, const AssertArrayValue<T>& assertValue)
{
	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_array);
	REQUIRE(retVal->IsSharedObject() == true);

	auto array = retVal->ToArray();
	REQUIRE(array != nullptr);
	auto internal = array->Internal();
	REQUIRE(internal != nullptr);
	auto& vars = internal->GetVariables();

	REQUIRE(internal->Length() == assertValue.m_targetValues.size());
	REQUIRE(vars.size() == assertValue.m_targetValues.size());

	auto targets = assertValue.m_targetValues;

	for (std::size_t i = 0; i < vars.size(); i++) {
		REQUIRE(vars[i]->GetValue() != nullptr);
		REQUIRE(vars[i]->GetValue()->HasOwner() == true);
		REQUIRE(vars[i]->GetValue()->Type() == assertValue.m_type);
		REQUIRE(GetTemplatedValue<T>(vars[i]->GetValue()) == targets[i]);
	}
}
