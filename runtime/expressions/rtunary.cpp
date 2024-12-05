#include "runtime/structure.hpp"
#include "runtime/functions/rtfunction.hpp"
#include "runtime/values/types/types.hpp"
#include "runtime/values/types/array_internal/array_builtin.hpp"

#include "runtime/values/simple_operators.hpp"

#include "runtime/variables.hpp"
#include "runtime/runtime.hpp"
#include "runtime/exceptions/exception.hpp"

#include "linter/expressions/ast.hpp"

#include <cassert>
#include <format>

static IValue* EvaluateNegation(IValue* operand);
static IValue* EvaluateIncrement(IValue* operand);
static IValue* EvaluateDecrement(IValue* operand);
static IValue* EvaluateLogicalNot(IValue* operand);

IValue* CRuntimeExpression::EvaluateUnary(CFunction* const thisFunction, const UnaryASTNode* node)
{
	auto operand = Evaluate(thisFunction, node->left.get());
	IValue* returnVal{ nullptr };

	if (node->IsNegation()) {
		returnVal = EvaluateNegation(operand);
	} else if (node->IsIncrement()) {
		returnVal = EvaluateIncrement(operand);
	} else if (node->IsDecrement()) {
		returnVal = EvaluateDecrement(operand);
	} else if (node->IsLogicalNot()) {
		returnVal = EvaluateLogicalNot(operand);
	}
	
	assert(returnVal);

	if (!operand->HasOwner())
		operand->Release();

	return returnVal;
}
IValue* EvaluateNegation(IValue* operand)
{
	if (operand->Type() == t_int) {
		return CProgramRuntime::AcquireNewValue<CIntValue>(-operand->AsInt());
	}
	
	if (operand->Type() == t_double) {
		return CProgramRuntime::AcquireNewValue<CDoubleValue>(-operand->AsDouble());
	}

	throw CRuntimeError(std::format("cannot negate a value of type \"{}\"", operand->TypeAsString()));
}
IValue* EvaluateIncrement(IValue* operand)
{
	if (!operand->HasOwner())
		throw CRuntimeError("cannot increment a temporary value");

	if (operand->Type() != t_int)
		throw CRuntimeError("the increment operand must have an int type");

	++operand->AsInt();
	return operand;
}
IValue* EvaluateDecrement(IValue* operand)
{
	if (!operand->HasOwner())
		throw CRuntimeError("cannot decrement a temporary value");

	if (operand->Type() != t_int)
		throw CRuntimeError("the decrement operand must have an int type");
	--operand->AsInt();
	return operand;
}
IValue* EvaluateLogicalNot(IValue* operand)
{
	if (!operand->IsBooleanConvertible())
		throw CRuntimeError(std::format("a value of type \"{}\" is not convertible to a boolean", operand->TypeAsString()));

	return CProgramRuntime::AcquireNewValue<CBooleanValue>(!operand->ToBoolean());
}
