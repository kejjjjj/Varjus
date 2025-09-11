#include "tests/tests.hpp"
#include "tests/utils.hpp"


#define PATH_PREFIX VarjusString(VSL("operators")) + VARJUS_DIRECTORY_SEPARATOR + VSL("logical")
#define JP(x) (VarjusString(PATH_PREFIX) + VARJUS_DIRECTORY_SEPARATOR + x)

TEST_CASE(("logical and operations")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("logical_and.var")));

	#if defined(__clang__)
		#pragma GCC diagnostic push
		#pragma GCC diagnostic ignored "-Wconstant-logical-operand"
	#endif
	AssertArray(retVal, AssertArrayValue<ASSERT_BOOL>(t_boolean, { 
		true && true,
		true && false,
		false && true,

		false, //undefined && undefined

		1 && 1,
		0 && 0,
		1 && 0,

		1.0 && 1.0,
		0.0 && 0.0,
		1.0 && 0.0
	}));
	#if defined(__clang__)
		#pragma GCC diagnostic pop
	#endif

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}

TEST_CASE(("logical or operations")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("logical_or.var")));

	#if defined(__clang__)
		#pragma GCC diagnostic push
		#pragma GCC diagnostic ignored "-Wconstant-logical-operand"
	#endif
		AssertArray(retVal, AssertArrayValue<ASSERT_BOOL>(t_boolean, {
			static_cast<bool>(true || true),
			static_cast<bool>(true || false),
			static_cast<bool>(false || true),

			static_cast<bool>(false), //undefined || undefined

			static_cast<bool>(1 || 1),
			static_cast<bool>(0 || 0),
			static_cast<bool>(1 || 0),

			static_cast<bool>(1.0 || 1.0),
			static_cast<bool>(0.0 || 0.0),
			static_cast<bool>(1.0 || 0.0)
		}));
	#if defined(__clang__)
		#pragma GCC diagnostic pop
	#endif

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}