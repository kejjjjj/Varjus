#include "tests/tests.hpp"
#include "tests/utils.hpp"


#define PATH_PREFIX VSL("primitive_types")
#define JP(x) (VarjusString(PATH_PREFIX) + DIRECTORY_SEPARATOR + x)

TEST_CASE(("Returns an undefined")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("undefined.var")));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_undefined);

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);

}
TEST_CASE(("Returns a boolean true")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("boolean.var")));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_boolean);
	REQUIRE(retVal->ToBoolean() == true);

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("Returns an integer 64")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("integer.var")));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 64);

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("Returns a uinteger 64")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("uinteger.var")));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_uint);
	REQUIRE(retVal->ToUInt() == 64);

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("Returns a double 64.0")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("double.var")));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_double);
	REQUIRE(retVal->ToDouble() == 64.0);

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("Returns a string Hello, World!")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("string.var")));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_string);
	REQUIRE(retVal->ToString() == VSL("Hello, World!"));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}


TEST_CASE(("Returns string length of Hello, World!")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("string_length.var")));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_uint);
	REQUIRE(retVal->ToUInt() == 13);

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}