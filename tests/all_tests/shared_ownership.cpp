#include "tests/tests.hpp"
#include "tests/utils.hpp"

#include "runtime/runtime.hpp"

#define PATH_PREFIX "shared_ownership"
#define JP(x) (std::string(PATH_PREFIX) + "\\" + x)

TEST_CASE("Array [0, 1] gets copied and then the copy gets a value 50 assigned to [0]") {

	auto retVal = TEST_ExecuteFile(JP("array.var"));

	AssertArray(retVal, AssertArrayValue<ASSERT_INT>{ t_int, {50, 1} });

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}

TEST_CASE("object {a: 0, b: 1} gets copied and then the copy gets a value 50 assigned to a")
{
	auto retVal = TEST_ExecuteFile(JP("object.var"));

	AssertObject(retVal, AssertObjectValue<ASSERT_INT>(t_int, 0u, { {"a", 50}, {"b", 1} }));

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}