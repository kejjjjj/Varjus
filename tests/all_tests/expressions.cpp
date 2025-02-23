#include "tests/tests.hpp"
#include "tests/utils.hpp"

#define PATH_PREFIX "expressions"
#define JP(x) (std::string(PATH_PREFIX) + DIRECTORY_SEPARATOR + x)


TEST_CASE("operator precedence tests in simple expressions")
{
	auto retVal = TEST_ExecuteFile(JP("operator_precedence.var"));

	AssertArray(retVal, AssertArrayValue<ASSERT_INT>(t_int, { 
		2 + 3 * 2,
		(2 + 3) * 2,
		1 * (7 > 2 * 3),
		((1 << 16) + 2) | 257,
		(((4 - 2) * 6) << 5) % 5
	}));

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}

TEST_CASE("operator precedence tests in sequences")
{
	auto retVal = TEST_ExecuteFile(JP("sequence.var"));

	AssertArray(retVal, AssertArrayValue<ASSERT_INT>(t_int, {
		(static_cast<void>(2 + 3), static_cast<void>(2), 4),
		(static_cast<void>(2 + 3), 4),
		2 + (static_cast<void>(2 + 3), 4),
		2 + (static_cast<void>(2 + 3), 4) + 3
	}));

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}