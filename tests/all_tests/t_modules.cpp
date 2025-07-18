#include "tests/tests.hpp"
#include "tests/utils.hpp"


#define PATH_PREFIX VSL("modules")
#define JP(x) (VarjusString(PATH_PREFIX) + VARJUS_DIRECTORY_SEPARATOR + x)

TEST_CASE(("Main imports global variable")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("main_imports_variable") + VARJUS_DIRECTORY_SEPARATOR + VSL("main.var")));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 50);

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("Main imports global variable and edits it")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("main_imports_variable") + VARJUS_DIRECTORY_SEPARATOR + VSL("main_imports_and_uses.var")));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 4);

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("Main imports function")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("main_imports_function") + VARJUS_DIRECTORY_SEPARATOR + VSL("main.var")));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_callable);
	//REQUIRE(retVal->ToInt() == 50);

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("Main imports function and calls it")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("main_imports_function") + VARJUS_DIRECTORY_SEPARATOR + VSL("main_calls_exported.var")));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 50);

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}

TEST_CASE(("Imported function edits lambda capture")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("lambda_captures_get_passed") + VARJUS_DIRECTORY_SEPARATOR + VSL("main.var")));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 50);

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}


TEST_CASE(("VarjusDouble import (main imports from a and a imports from b)")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("double_import") + VARJUS_DIRECTORY_SEPARATOR + VSL("main.var")));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 151);

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}

TEST_CASE(("Main imports variable and func then the variable gets edited in func")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("main_imports_multiple") + VARJUS_DIRECTORY_SEPARATOR + VSL("main.var")));

	AssertArray(retVal, AssertArrayValue<ASSERT_INT>(t_int, { 50, 111 }));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}

TEST_CASE(("Main and other call array methods")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("calling_methods_in_separate_files") + VARJUS_DIRECTORY_SEPARATOR + VSL("main.var")));

	AssertArray(retVal, AssertArrayValue<ASSERT_INT>(t_int, { 50, 2, 3}));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("Module throws and main catches it")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("module_throws_and_main_catches") + VARJUS_DIRECTORY_SEPARATOR + VSL("main.var")));

	AssertArray(retVal, AssertArrayValue<ASSERT_INT>(t_int, { 1, 2, 3 }));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}

TEST_CASE(("Relative path forward")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("relative_path_forward") + VARJUS_DIRECTORY_SEPARATOR + VSL("main.var")));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 21);

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("Relative path backward")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("relative_path_backward") + 
		VARJUS_DIRECTORY_SEPARATOR + VSL("directory") + 
		VARJUS_DIRECTORY_SEPARATOR + VSL("main.var")));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 22);

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}