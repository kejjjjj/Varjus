#include "tests/tests.hpp"
#include "tests/utils.hpp"


#define PATH_PREFIX "primitive_types_as_variables"
#define JP(x) (std::string(PATH_PREFIX) + "\\" + x)

TEST_CASE("Returns an undefined in variable context") {

	auto retVal = TEST_ExecuteFile(JP("undefined.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_undefined);

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);

}
TEST_CASE("Returns a boolean true in variable context") {

	auto retVal = TEST_ExecuteFile(JP("boolean.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_boolean);
	REQUIRE(retVal->ToBoolean() == true);

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
TEST_CASE("Returns an integer 64 in variable context") {

	auto retVal = TEST_ExecuteFile(JP("integer.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 64);

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}

TEST_CASE("Returns a VarjusDouble 64.0 in variable context") {

	auto retVal = TEST_ExecuteFile(JP("VarjusDouble.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_double);
	REQUIRE(retVal->ToDouble() == 64.0);

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
TEST_CASE("Returns a string Hello, World! in variable context") {

	auto retVal = TEST_ExecuteFile(JP("string.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_string);
	REQUIRE(retVal->ToString() == "Hello, World!");

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
TEST_CASE("Returns string length of Hello, World! in variable context") {

	auto retVal = TEST_ExecuteFile(JP("string_length.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 13);

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}