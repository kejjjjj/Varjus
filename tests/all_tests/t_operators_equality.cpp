#include "tests/tests.hpp"
#include "tests/utils.hpp"


#define PATH_PREFIX VarjusString(VSL("operators")) + VARJUS_DIRECTORY_SEPARATOR + VSL("equality")
#define JP(x) (VarjusString(PATH_PREFIX) + VARJUS_DIRECTORY_SEPARATOR + x)

TEST_CASE(("equality operations")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("equality.var")));

	AssertArray(retVal, AssertArrayValue<ASSERT_BOOL>(t_boolean, { 
		1 == 1,
		1 == true,
		1 == 1.0,
		1 == 1u,

		1 == 2,
		1 == false,
		1 == 1.1,

		true,  //VSL("hello") == VSL("hello")
		false, //VSL("hello1") == VSL("hello")

		true,  //undefined == undefined

		false, //main == main

		false, //fn(){ return 1; } == fn() { return 1; }

		false, //[] == []
		false, //{} == {}

	}));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}

TEST_CASE(("strict equality operations")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("strict_equality.var")));

	AssertArray(retVal, AssertArrayValue<ASSERT_BOOL>(t_boolean, {
		1 == 1,
		false, //1 === true
		false, //1 === 1.0
		false, 

		1 == 2,
		false, //1 === false
		false, // 1 === 1.1

		true,  //VSL("hello") === VSL("hello")
		false, //VSL("hello1") === VSL("hello")

		true,  //undefined === undefined

		false, //main === main

		false, //fn(){ return 1; } === fn() { return 1; }

		false, //[] ===[]
		false, //{} === {}

		}));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}

TEST_CASE(("unequality operations")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("unequality.var")));

	AssertArray(retVal, AssertArrayValue<ASSERT_BOOL>(t_boolean, {
		1 != 1,
		1 != true,
		1 != 1.0,
		1 != 1u,

		1 != 2,
		1 != false,
		1 != 1.1,

		false, //VSL("hello") != VSL("hello")
		true,  //VSL("hello1") != VSL("hello")

		false, //undefined != undefined

		true, //main != main

		true, //fn(){ return 1; } != fn() { return 1; }

		true, //[] != []
		true, //{} != {}

	}));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}

TEST_CASE(("strict unequality operations")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("strict_unequality.var")));

	AssertArray(retVal, AssertArrayValue<ASSERT_BOOL>(t_boolean, {
		1 != 1,
		true, // 1 !== true
		true, // 1 !== 1.0 
		true,

		1 != 2,
		1 != false, // 1 !== false
		1 != 1.1,

		false, //VSL("hello") !== VSL("hello")
		true,  //VSL("hello1") !== VSL("hello")

		false, //undefined !== undefined

		true, //main !== main

		true, //fn(){ return 1; } !== fn() { return 1; }

		true, //[] !== []
		true, //{} !== {}

	}));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
