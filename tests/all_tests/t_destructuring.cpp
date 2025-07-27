#include "tests/tests.hpp"
#include "tests/utils.hpp"

#include "varjus_api/internal/structure.hpp"
#include "varjus_api/internal/runtime.hpp"

#define PATH_PREFIX VSL("destructuring")
#define JP(x) (VarjusString(PATH_PREFIX) + VARJUS_DIRECTORY_SEPARATOR + x)


TEST_CASE(("const [x, y] = [1, 2]")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("array_no_rest.var")));

	AssertArray(retVal, AssertArrayValue<ASSERT_INT>(t_int, { 1,2 }));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("const [x, y] = [1, 2, 3, 4]")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("array_destructure_half.var")));

	AssertArray(retVal, AssertArrayValue<ASSERT_INT>(t_int, { 5,2 }));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("const [x, y, ...rest] = [1u, 2u, 3u, 4u]")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("array_rest.var")));

	AssertArray(retVal, AssertArrayValue<ASSERT_INT>(t_uint, { 1, 2, 3, 4, 2 }));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("const [...rest] = [1u, 2u, 3u, 4u]")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("array_only_rest.var")));

	AssertArray(retVal, AssertArrayValue<ASSERT_INT>(t_uint, { 1, 2, 3, 4, 4 }));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("const [x, y, ...rest] = [1u, 2u]")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("array_empty_rest.var")));

	AssertArray(retVal, AssertArrayValue<ASSERT_INT>(t_uint, { 1, 2, 0 }));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("const [...rest] = []")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("array_only_empty_rest.var")));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_uint);
	REQUIRE(retVal->ToUInt() == 0u);

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("const { a, b } = {a: 2, b: 3}")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("object.var")));

	AssertArray(retVal, AssertArrayValue<ASSERT_INT>(t_int, { 2, 3 }));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("const { a, b } = {a: 1, b: 3, c: 4, d: 5}")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("object_destructure_half.var")));

	AssertArray(retVal, AssertArrayValue<ASSERT_INT>(t_int, { 1, 3 }));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("const { a, d } = {a: 1, b: 3, c: [], d: 5}")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("object_destructure_first_last.var")));

	AssertArray(retVal, AssertArrayValue<ASSERT_INT>(t_int, { 1, 5 }));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("destructured array ranged for loop")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("array_no_rest_ranged.var")));

	AssertArray(retVal, AssertArrayValue<ASSERT_INT>(t_int, { 3, 5 }));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("destructured array ranged for loop with rest")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("array_rest_ranged.var")));

	AssertArray(retVal, AssertArrayValue<ASSERT_INT>(t_int, { 3, 7, 5, 9 }));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("destructured object ranged for loop")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("object_ranged.var")));

	AssertArray(retVal, AssertArrayValue<ASSERT_INT>(t_int, { 11, 15 }));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("destructured array for loop")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("array_for_loop.var")));

	AssertArray(retVal, AssertArrayValue<ASSERT_INT>(t_int, { 1, 2, 3, 4 }));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("destructured object for loop")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("object_for_loop.var")));

	AssertArray(retVal, AssertArrayValue<ASSERT_INT>(t_int, { 5, 6, 7, 8 }));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}