#include "tests/tests.hpp"
#include "tests/catch_amalgamated.hpp"
#include "runtime/structure.hpp"
#include "runtime/runtime.hpp"
#include "runtime/modules/rtmodule.hpp"

#define PATH_PREFIX "primitive_types"
#define JP(x) (std::string(PATH_PREFIX) + "\\" + x)

TEST_CASE("ReturnsUndefined") {

	auto retVal = TEST_ExecuteFile(JP("undefined.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_undefined);

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);

}
TEST_CASE("ReturnsTrue") {

	auto retVal = TEST_ExecuteFile(JP("boolean.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_boolean);
	REQUIRE(retVal->ToBoolean() == true);

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
TEST_CASE("ReturnsIntegerWithValue64") {

	auto retVal = TEST_ExecuteFile(JP("integer.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 64);

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
}

TEST_CASE("ReturnsDoubleWithValue64.0") {

	auto retVal = TEST_ExecuteFile(JP("double.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_double);
	REQUIRE(retVal->ToDouble() == 64.0);

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
TEST_CASE("StringReturnsHelloWorld") {

	auto retVal = TEST_ExecuteFile(JP("string.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_string);
	REQUIRE(retVal->ToString() == "Hello, World!");

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
TEST_CASE("ArrayIs [ 0, 1, 2 ]") {

	auto retVal = TEST_ExecuteFile(JP("array.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_array);

	auto array = retVal->ToArray();
	REQUIRE(array != nullptr);
	auto internal = array->Internal();
	REQUIRE(internal != nullptr);
	auto& vars = internal->GetVariables();

	REQUIRE(internal->Length() == 3);
	REQUIRE(vars.size() == 3);

	for (std::size_t i = 0; i < vars.size(); i++) {
		REQUIRE(vars[i]->GetValue() != nullptr);
		REQUIRE(vars[i]->GetValue()->Type() == t_int);
		REQUIRE(vars[i]->GetValue()->ToInt() == i);
	}

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
TEST_CASE("ObjectIs { a: 1, b: 2, c: 3 }") {

	auto retVal = TEST_ExecuteFile(JP("object.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_object);

	auto object = retVal->ToObject();
	REQUIRE(object != nullptr);
	auto internal = object->Internal();
	REQUIRE(internal != nullptr);
	auto& aggregate = internal->Get();
	auto& items = aggregate.Iterator();

	REQUIRE(items.size() == 3);
	
	auto m = CProgramRuntime::GetModuleByIndex(0);
	auto& allMembers = m->GetContext()->m_oAllMembers;
	std::vector<std::pair<std::string, int>> v({ {"a", 1}, {"b", 2}, {"c", 3}});

	for (auto& [name, value] : v) {
		REQUIRE(allMembers.Contains(name));
		IValue* result  = aggregate.ElementLookupNoExcept(allMembers[name]);
		REQUIRE(result->HasOwner() == true);
		REQUIRE(result != nullptr);
		REQUIRE(result->Type() == t_int);
		REQUIRE(result->ToInt() == value);
	}

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);

}
TEST_CASE("CallableHasValue") {

	auto retVal = TEST_ExecuteFile(JP("callable.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_callable);

	auto c = retVal->ToCallable();
	REQUIRE(c != nullptr);
	auto internal = c->Internal();
	REQUIRE(internal != nullptr);

	REQUIRE(internal->GetCaptures().empty() == true);
	REQUIRE(internal->m_pCallable != nullptr);
	REQUIRE(internal->m_pCallable->GetName() == "main");
	REQUIRE(internal->m_pCallable->GetModuleIndex() == 0);

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}