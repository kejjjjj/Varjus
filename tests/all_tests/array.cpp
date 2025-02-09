#include "tests/tests.hpp"
#include "tests/utils.hpp"

#define PATH_PREFIX "array"
#define JP(x) (std::string(PATH_PREFIX) + "\\" + x)

TEST_CASE("Array.sort([4,2,6,3,1,5])") {

	auto retVal = TEST_ExecuteFile(JP("sort.var"));

	AssertArray(retVal, AssertArrayValue<ASSERT_INT>(t_int, { 1,2,3,4,5,6 }));

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
TEST_CASE("Array[1..10].resize(5)") {

	auto retVal = TEST_ExecuteFile(JP("resize_shrink.var"));

	AssertArray(retVal, AssertArrayValue<ASSERT_INT>(t_int, { 1,2,3,4,5 }));

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
TEST_CASE("Array[].resize(5)") {

	auto retVal = TEST_ExecuteFile(JP("resize_grow.var"));

	//false == undefined in this case
	AssertArray(retVal, AssertArrayValue<ASSERT_BOOL>(t_boolean, { false, false, false, false, false }));

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
TEST_CASE("Array[1..10].filter()") {

	auto retVal = TEST_ExecuteFile(JP("filter.var"));

	//false == undefined in this case
	AssertArray(retVal, AssertArrayValue<ASSERT_INT>(t_int, { 5,6,7,8,9,10 }));

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
TEST_CASE("Array[1..5].map()") {

	auto retVal = TEST_ExecuteFile(JP("map.var"));

	//false == undefined in this case
	AssertArray(retVal, AssertArrayValue<ASSERT_INT>(t_int, { 2,4,6,8,10 }));

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
TEST_CASE("Array[1..5].fill(hello)") {

	auto retVal = TEST_ExecuteFile(JP("fill.var"));

	//false == undefined in this case
	AssertArray(retVal, AssertArrayValue<ASSERT_STRING>(t_string, { "hello", "hello", "hello", "hello", "hello" }));

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
TEST_CASE("Array[1..5].any()") {

	auto retVal = TEST_ExecuteFile(JP("any.var"));

	//false == undefined in this case
	AssertArray(retVal, AssertArrayValue<ASSERT_BOOL>(t_boolean, { true, false, true }));

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
TEST_CASE("Array[1..5].all()") {

	auto retVal = TEST_ExecuteFile(JP("all.var"));

	//false == undefined in this case
	AssertArray(retVal, AssertArrayValue<ASSERT_BOOL>(t_boolean, { false, false, true }));

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
TEST_CASE("Array[1..5].reversed()") {

	auto retVal = TEST_ExecuteFile(JP("reversed.var"));

	AssertArray(retVal, AssertArrayValue<ASSERT_INT>(t_int, { 5,4,3,2,1 }));

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
TEST_CASE("Array[1..5].contains()") {

	auto retVal = TEST_ExecuteFile(JP("contains.var"));

	//false == undefined in this case
	AssertArray(retVal, AssertArrayValue<ASSERT_BOOL>(t_boolean, { true, false, true }));

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}

TEST_CASE("Array[].join()") {

	auto retVal = TEST_ExecuteFile(JP("join.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_string);
	REQUIRE(retVal->IsSharedObject() == false);
	
	REQUIRE(retVal->ToString() == "hello, world, hello1, world1");

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
TEST_CASE("Array[].slice(3, 5)") {

	auto retVal = TEST_ExecuteFile(JP("slice.var"));
	
	AssertArray(retVal, AssertArrayValue<ASSERT_STRING>(t_string, { "dd", "ee" }));

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
TEST_CASE("Array[1..6].find()") {

	auto retVal = TEST_ExecuteFile(JP("find.var"));

	//false == undefined in this case
	AssertArray(retVal, AssertArrayValue<ASSERT_BOOL>(t_boolean, { true, false }));

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
TEST_CASE("Array[].find_last()") {

	auto retVal = TEST_ExecuteFile(JP("find_last.var"));

	AssertArray(retVal, AssertArrayValue<ASSERT_STRING>(t_string, { "2_last", "bad" }));

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
TEST_CASE("Array[1..6].find_index()") {

	auto retVal = TEST_ExecuteFile(JP("find_index.var"));

	AssertArray(retVal, AssertArrayValue<ASSERT_INT>(t_int, { 4, -1 }));

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
TEST_CASE("Array[].find_last_index()") {

	auto retVal = TEST_ExecuteFile(JP("find_last_index.var"));

	AssertArray(retVal, AssertArrayValue<ASSERT_INT>(t_int, { 7, -1 }));

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
TEST_CASE("Array[1,2,3].push(4)") {

	auto retVal = TEST_ExecuteFile(JP("push.var"));

	AssertArray(retVal, AssertArrayValue<ASSERT_INT>(t_int, { 4, 1, 2, 3, 4, 4 }));

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
TEST_CASE("Array[1,2,3].push_front(4)") {

	auto retVal = TEST_ExecuteFile(JP("push_front.var"));

	AssertArray(retVal, AssertArrayValue<ASSERT_INT>(t_int, { 4, 4, 1, 2, 3, 4 }));

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
TEST_CASE("Array[1,2,3].pop()") {

	auto retVal = TEST_ExecuteFile(JP("pop.var"));

	//false == undefined in this case
	AssertArray(retVal, AssertArrayValue<ASSERT_INT>(t_int, { 3, 1, 2, 2 }));

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
TEST_CASE("Array[1,2,3].pop_front()") {

	auto retVal = TEST_ExecuteFile(JP("pop_front.var"));

	//false == undefined in this case
	AssertArray(retVal, AssertArrayValue<ASSERT_INT>(t_int, { 1, 2, 3, 2 }));

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}