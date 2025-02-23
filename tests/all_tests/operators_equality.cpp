#include "tests/tests.hpp"
#include "tests/utils.hpp"


#define PATH_PREFIX "operators\\equality"
#define JP(x) (std::string(PATH_PREFIX) + DIRECTORY_SEPARATOR + x)

TEST_CASE("equality operations") {

	auto retVal = TEST_ExecuteFile(JP("equality.var"));

	AssertArray(retVal, AssertArrayValue<ASSERT_BOOL>(t_boolean, { 
		1 == 1,
		1 == true,
		1 == 1.0,
		1 == 1u,

		1 == 2,
		1 == false,
		1 == 1.1,

		true,  //"hello" == "hello"
		false, //"hello1" == "hello"

		true,  //undefined == undefined

		false, //main == main

		false, //fn(){ return 1; } == fn() { return 1; }

		false, //[] == []
		false, //{} == {}

	}));

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}

TEST_CASE("strict equality operations") {

	auto retVal = TEST_ExecuteFile(JP("strict_equality.var"));

	AssertArray(retVal, AssertArrayValue<ASSERT_BOOL>(t_boolean, {
		1 == 1,
		false, //1 === true
		false, //1 === 1.0
		false, 

		1 == 2,
		false, //1 === false
		false, // 1 === 1.1

		true,  //"hello" === "hello"
		false, //"hello1" === "hello"

		true,  //undefined === undefined

		false, //main === main

		false, //fn(){ return 1; } === fn() { return 1; }

		false, //[] ===[]
		false, //{} === {}

		}));

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}

TEST_CASE("unequality operations") {

	auto retVal = TEST_ExecuteFile(JP("unequality.var"));

	AssertArray(retVal, AssertArrayValue<ASSERT_BOOL>(t_boolean, {
		1 != 1,
		1 != true,
		1 != 1.0,
		1 != 1u,

		1 != 2,
		1 != false,
		1 != 1.1,

		false, //"hello" != "hello"
		true,  //"hello1" != "hello"

		false, //undefined != undefined

		true, //main != main

		true, //fn(){ return 1; } != fn() { return 1; }

		true, //[] != []
		true, //{} != {}

	}));

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}

TEST_CASE("strict unequality operations") {

	auto retVal = TEST_ExecuteFile(JP("strict_unequality.var"));

	AssertArray(retVal, AssertArrayValue<ASSERT_BOOL>(t_boolean, {
		1 != 1,
		true, // 1 !== true
		true, // 1 !== 1.0 
		true,

		1 != 2,
		1 != false, // 1 !== false
		1 != 1.1,

		false, //"hello" !== "hello"
		true,  //"hello1" !== "hello"

		false, //undefined !== undefined

		true, //main !== main

		true, //fn(){ return 1; } !== fn() { return 1; }

		true, //[] !== []
		true, //{} !== {}

	}));

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
