#include "tests/tests.hpp"
#include "tests/utils.hpp"

#include "api/internal/structure.hpp"
#include "api/internal/runtime.hpp"

#define PATH_PREFIX "composite_types"
#define JP(x) (std::string(PATH_PREFIX) + "\\" + x)

TEST_CASE("returns an array [ 0, 1, 2 ]") {

	auto retVal = TEST_ExecuteFile(JP("array.var"));

	AssertArray(retVal, AssertArrayValue<ASSERT_INT>{ t_int, { 0, 1, 2 } });

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
TEST_CASE("Returns an object { a: 1, b: 2, c: 3 }") {

	auto retVal = TEST_ExecuteFile(JP("object.var"));
	AssertObject(retVal, AssertObjectValue<ASSERT_INT>(t_int, { {"a", 1}, {"b", 2}, {"c", 3} }));

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);

}
TEST_CASE("Returns a callable") {

	auto retVal = TEST_ExecuteFile(JP("callable.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_callable);
	REQUIRE(retVal->IsSharedObject() == true);

	auto c = retVal->ToCallable();
	REQUIRE(c != nullptr);
	auto internal = c->Internal();
	REQUIRE(internal != nullptr);

	REQUIRE(internal->GetCaptures().empty() == true);
	REQUIRE(internal->GetCaptures().empty() == true);
	REQUIRE(internal->GetCallable()->FunctionType() == fn_regular);

	auto func = dynamic_cast<CRuntimeFunction*>(internal->GetCallable());

	REQUIRE(func != nullptr);
	REQUIRE(func->GetName() == "main");
	REQUIRE(func->GetModuleIndex() == 0);

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}

TEST_CASE("Returns a lambda") {
	auto retVal = TEST_ExecuteFile(JP("lambda.var"));

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
	REQUIRE(func->GetName() == "lambda");
	REQUIRE(func->GetModuleIndex() == 0);

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}

TEST_CASE("Temporary array [50,1,2] gets indexed at 0") {

	auto retVal = TEST_ExecuteFile(JP("subscript.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 50);

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}

TEST_CASE("Temporary object {a: 50, b: 1, c: 2} gets accessed at a") {

	auto retVal = TEST_ExecuteFile(JP("member_access.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 50);

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
TEST_CASE("Temporary callable return50 gets called") {

	auto retVal = TEST_ExecuteFile(JP("function_call.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 50);

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}
TEST_CASE("Temporary lambda fn() { return 50; } gets called") {

	auto retVal = TEST_ExecuteFile(JP("lambda_call.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 50);

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}

TEST_CASE("Returns array length of [1,2,3]") {

	auto retVal = TEST_ExecuteFile(JP("array_length.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 3);

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}

TEST_CASE("Temporary arrow function () => 2") {

	auto retVal = TEST_ExecuteFile(JP("arrow_function.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_callable);

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}

TEST_CASE("Temporary arrow function call () => 2") {

	auto retVal = TEST_ExecuteFile(JP("arrow_function_call.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 2);

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}

TEST_CASE("Temporary arrow function call with args ((a, b) => a + b)(2, 4)") {

	auto retVal = TEST_ExecuteFile(JP("arrow_function_call_with_args.var"));

	REQUIRE(retVal != nullptr);
	REQUIRE(retVal->Type() == t_int);
	REQUIRE(retVal->ToInt() == 6);

	REQUIRE(retVal->HasOwner() == false);
	retVal->Release();
	REQUIRE(CProgramRuntime::HasLeaks() == false);
}