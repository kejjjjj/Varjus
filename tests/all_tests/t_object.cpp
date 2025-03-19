#include "tests/tests.hpp"
#include "tests/utils.hpp"

#define PATH_PREFIX VSL("object")
#define JP(x) (VarjusString(PATH_PREFIX) + DIRECTORY_SEPARATOR + x)

TEST_CASE(("Object.set")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("set.var")));

	AssertObject(retVal, AssertObjectValue<ASSERT_STRING>(t_string, { 
		{VSL("key0"), VSL("value0") },
		{VSL("key1"), VSL("value1") },
		{VSL("key2"), VSL("value2_edited") },
		{VSL("key3"), VSL("value3") }
	}));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("Object.remove")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("remove.var")));

	AssertArray(retVal, AssertArrayValue<ASSERT_STRING>(t_string, {
		VSL("true"), VSL("false"), VSL("false"), VSL("2"), VSL("key1value1"), VSL("key2value2")
	}));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("Object.keys")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("keys.var")));

	AssertArray(retVal, AssertArrayValue<ASSERT_STRING>(t_string, {
		VSL("key0"),
		VSL("key1"),
		VSL("key2")
	}));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("Object.values")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("values.var")));

	AssertArray(retVal, AssertArrayValue<ASSERT_STRING>(t_string, {
		VSL("value0"),
		VSL("value1"),
		VSL("value2") 
	}));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("Object.contains")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("contains.var")));

	AssertArray(retVal, AssertArrayValue<ASSERT_BOOL>(t_boolean, {
		true,
		false,
		true
	}));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("Object.to_array")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("to_array.var")));

	AssertArray(retVal, AssertArrayValue<ASSERT_STRING>(t_string, {
		VSL("key0value0"), VSL("key1value1"), VSL("key2value2")
	}));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}