#include "tests/tests.hpp"
#include "tests/utils.hpp"

#include "varjus_api/internal/structure.hpp"
#include "varjus_api/internal/runtime.hpp"
#define PATH_PREFIX VSL("composite_types_as_variables")
#define JP(x) (VarjusString(PATH_PREFIX) + DIRECTORY_SEPARATOR + x)

TEST_CASE(("returns an array [ 0, 1, 2 ] in variable context")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("array.var")));

	AssertArray(retVal, AssertArrayValue<ASSERT_INT>{ t_int, { 0, 1, 2 } });

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("Returns an object { a: 1, b: 2, c: 3 } in variable context")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("object.var")));

	AssertObject(retVal, AssertObjectValue<ASSERT_INT>(t_int, { {VSL("a"), 1}, {VSL("b"), 2}, {VSL("c"), 3} }));

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);

}
TEST_CASE(("returns a callable in variable context")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("callable.var")));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_callable);
	REQUIRE(retVal->IsSharedObject() == true);

	auto c = retVal->ToCallable();
	REQUIRE(c != nullptr);
	auto internal = c->Internal();
	REQUIRE(internal != nullptr);

	REQUIRE(internal->GetCaptures().empty() == true);
	REQUIRE(internal->GetCallable()->FunctionType() == fn_regular);

	auto func = dynamic_cast<CRuntimeFunction*>(internal->GetCallable());

	REQUIRE(func != nullptr);
	REQUIRE(func->GetName() == VSL("main"));
	REQUIRE(func->GetModuleIndex() == 0);

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}

TEST_CASE(("returns a lambda in variable context")) {
	auto retVal = TEST_ExecuteFile(JP(VSL("lambda.var")));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_callable);
	REQUIRE(retVal->IsSharedObject() == true);

	auto c = retVal->ToCallable();
	REQUIRE(c != nullptr);
	auto internal = c->Internal();
	REQUIRE(internal != nullptr);

	REQUIRE(internal->GetCaptures().empty() == true);
	REQUIRE(internal->GetCallable()->FunctionType() == fn_regular);

	auto func = dynamic_cast<CRuntimeFunction*>(internal->GetCallable());

	REQUIRE(func != nullptr);
	REQUIRE(func->GetName() == VSL("lambda"));
	REQUIRE(func->GetModuleIndex() == 0);

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}

TEST_CASE(("Variable array [50,1,2] gets indexed at 0")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("subscript.var")));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 50);

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}

TEST_CASE(("Variable object {a: 50, b: 1, c: 2} gets accessed at a")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("member_access.var")));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 50);

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("Variable callable return50 gets called")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("function_call.var")));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 50);

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("Variable lambda fn() { return 50; } gets called")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("lambda_call.var")));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 50);

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}

TEST_CASE(("Returns array length of [1,2,3] in variable context")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("array_length.var")));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_uint);
	REQUIRE(retVal->ToUInt() == 3);

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("Returns object length of {a: 0, b: 1, c: 2} in variable context")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("object_length.var")));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_uint);
	REQUIRE(retVal->ToUInt() == 3);

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}
TEST_CASE(("Arrow function () => 2 in variable context")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("arrow_function.var")));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_callable);

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}

TEST_CASE(("Arrow function call () => 2 in variable context")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("arrow_function_call.var")));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 2);

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}

TEST_CASE(("Arrow function call with args ((a, b) => a + b)(2, 4) in variable context")) {

	auto retVal = TEST_ExecuteFile(JP(VSL("arrow_function_call_with_args.var")));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 6);

	REQUIRE(retVal->HasOwner() == false);
	TEST_END(retVal);
}