#pragma once


#include "varjus_api/types/types.hpp"
#include "varjus_api/internal/globalDefinitions.hpp"
#include "runtime/modules/rtmodule.hpp"
#include "varjus_api/internal/runtime.hpp"
#include "varjus_api/internal/structure.hpp"

#include "tests.hpp"
#include "utils.hpp"


#include <vector>

#ifdef __linux__
#define DIRECTORY_SEPARATOR VSL("/")
#else
#define DIRECTORY_SEPARATOR VSL("\\")
#endif

using ASSERT_BOOL = bool;
using ASSERT_INT = VarjusInt;
using ASSERT_UINT = VarjusUInt;
using ASSERT_DOUBLE = VarjusDouble;
using ASSERT_STRING = VarjusString;
using ASSERT_UNDEFINED = void;

template<typename T>
struct AssertObjectValue
{
	EValueType m_type;
	std::vector<std::pair<VarjusString, T>> m_targetValues;
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
	if constexpr (std::is_same_v<ASSERT_BOOL, T> || std::is_same_v<ASSERT_UNDEFINED, T>)
		return t->ToBoolean();
	else if constexpr (std::is_same_v<ASSERT_INT, T>)
		return t->ToInt();
	else if constexpr (std::is_same_v<ASSERT_UINT, T>)
		return t->ToUInt();
	else if constexpr (std::is_same_v<ASSERT_DOUBLE, T>)
		return t->ToDouble();
	else if constexpr (std::is_same_v<ASSERT_STRING, T>)
		return t->ToString();
	else
		static_assert(false, VSL("bad type"));
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

	REQUIRE(retVal->GetAllocator());
	REQUIRE(retVal->GetAllocator()->GetInformation());

	auto& allMembers = retVal->GetAllocator()->GetInformation()->m_oAllMembers;

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
