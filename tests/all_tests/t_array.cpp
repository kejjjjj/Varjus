#include "tests/tests.hpp"
#include "tests/utils.hpp"

#define PATH_PREFIX VSL("array")
#define JP(x) (VarjusString(PATH_PREFIX) + DIRECTORY_SEPARATOR + x)

TEST_CASE(("Array.sort([4,2,6,3,1,5])")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("sort.var")));

	AssertArray(retVal, AssertArrayValue<ASSERT_INT>(t_int, { 1,2,3,4,5,6 }));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("Array[1..10].resize(5)")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("resize_shrink.var")));

	AssertArray(retVal, AssertArrayValue<ASSERT_INT>(t_int, { 1,2,3,4,5 }));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("Array[].resize(5)")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("resize_grow.var")));

	//false == undefined in this case
	AssertArray(retVal, AssertArrayValue<ASSERT_BOOL>(t_boolean, { false, false, false, false, false }));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("Array[1..10].filter()")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("filter.var")));

	//false == undefined in this case
	AssertArray(retVal, AssertArrayValue<ASSERT_INT>(t_int, { 5,6,7,8,9,10 }));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("Array[1..5].transform()")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("transform.var")));

	//false == undefined in this case
	AssertArray(retVal, AssertArrayValue<ASSERT_INT>(t_int, { 2,4,6,8,10 }));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("Array[1..5].fill(hello)")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("fill.var")));

	//false == undefined in this case
	AssertArray(retVal, AssertArrayValue<ASSERT_STRING>(t_string, { VSL("hello"), VSL("hello"), VSL("hello"), VSL("hello"), VSL("hello") }));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("Array[1..5].any()")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("any.var")));

	//false == undefined in this case
	AssertArray(retVal, AssertArrayValue<ASSERT_BOOL>(t_boolean, { true, false, true }));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("Array[1..5].all()")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("all.var")));

	//false == undefined in this case
	AssertArray(retVal, AssertArrayValue<ASSERT_BOOL>(t_boolean, { false, false, true }));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("Array[1..5].reversed()")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("reversed.var")));

	AssertArray(retVal, AssertArrayValue<ASSERT_INT>(t_int, { 5,4,3,2,1 }));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("Array[1..5].contains()")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("contains.var")));

	//false == undefined in this case
	AssertArray(retVal, AssertArrayValue<ASSERT_BOOL>(t_boolean, { true, false, true }));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}

TEST_CASE(("Array[].join()")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("join.var")));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_string);
	REQUIRE(retVal->IsSharedObject() == false);
	
	REQUIRE(retVal->ToString() == VSL("hello, world, hello1, world1"));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("Array[].slice(3, 5)")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("slice.var")));
	
	AssertArray(retVal, AssertArrayValue<ASSERT_STRING>(t_string, { VSL("dd"), VSL("ee") }));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("Array[1..6].find()")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("find.var")));

	//false == undefined in this case
	AssertArray(retVal, AssertArrayValue<ASSERT_BOOL>(t_boolean, { true, false }));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("Array[].find_last()")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("find_last.var")));

	AssertArray(retVal, AssertArrayValue<ASSERT_STRING>(t_string, { VSL("2_last"), VSL("bad") }));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("Array[1..6].find_index()")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("find_index.var")));

	AssertArray(retVal, AssertArrayValue<ASSERT_INT>(t_int, { 4, -1 }));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("Array[].find_last_index()")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("find_last_index.var")));

	AssertArray(retVal, AssertArrayValue<ASSERT_INT>(t_int, { 7, -1 }));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("Array[1,2,3].push(4)")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("push.var")));

	AssertArray(retVal, AssertArrayValue<ASSERT_STRING>(t_string, { VSL("4"), VSL("1"), VSL("2"), VSL("3"), VSL("4"), VSL("4") }));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("Array[1,2,3].push_front(4)")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("push_front.var")));

	AssertArray(retVal, AssertArrayValue<ASSERT_STRING>(t_string, { VSL("4"), VSL("4"), VSL("1"), VSL("2"), VSL("3"), VSL("4") }));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("Array[1,2,3].pop()")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("pop.var")));

	//false == undefined in this case
	AssertArray(retVal, AssertArrayValue<ASSERT_STRING>(t_string, { VSL("3"), VSL("1"), VSL("2"), VSL("2") }));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("Array[1,2,3].pop_front()")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("pop_front.var")));

	//false == undefined in this case
	AssertArray(retVal, AssertArrayValue<ASSERT_STRING>(t_string, { VSL("1"), VSL("2"), VSL("3"), VSL("2") }));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("Array[4,2,6,3,1,5].for_each()")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("for_each.var")));

	AssertArray(retVal, AssertArrayValue<ASSERT_INT>(t_int, { 4,2,6,3,1,5 }));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("Array[1,2,3,4,5].accumulate(a + b, 0)")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("accumulate.var")));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 15);

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}