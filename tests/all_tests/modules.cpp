#include "tests/tests.hpp"
#include "tests/utils.hpp"


#define PATH_PREFIX "modules"
#define JP(x) (std::string(PATH_PREFIX) + "\\" + x)

TEST_CASE("Main imports global variable") {

	auto retVal = TEST_ExecuteFile(JP("main_imports_variable\\main.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 50);

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
TEST_CASE("Main imports global variable and edits it") {

	auto retVal = TEST_ExecuteFile(JP("main_imports_variable\\main_imports_and_uses.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 4);

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
TEST_CASE("Main imports function") {

	auto retVal = TEST_ExecuteFile(JP("main_imports_function\\main.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_callable);
	//REQUIRE(retVal->ToInt() == 50);

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
TEST_CASE("Main imports function and calls it") {

	auto retVal = TEST_ExecuteFile(JP("main_imports_function\\main_calls_exported.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 50);

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}

TEST_CASE("Imported function edits lambda capture") {

	auto retVal = TEST_ExecuteFile(JP("lambda_captures_get_passed\\main.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 50);

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}


TEST_CASE("Double import (main imports from a and a imports from b)") {

	auto retVal = TEST_ExecuteFile(JP("double_import\\main.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 151);

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}

TEST_CASE("Main imports variable and func then the variable gets edited in func") {

	auto retVal = TEST_ExecuteFile(JP("main_imports_multiple\\main.var"));

	AssertArray(retVal, AssertArrayValue<ASSERT_INT>(t_int, { 50, 111 }));

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}