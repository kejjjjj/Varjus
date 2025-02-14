#include "api/types/operators/default_operators.hpp"
#include "api/types/types.hpp"

#include "runtime/functions/rtfunction.hpp"
#include "runtime/structure.hpp"
#include "runtime/exceptions/exception.hpp"
#include "runtime/runtime.hpp"
#include "runtime/variables.hpp"

#include "linter/expressions/ast.hpp"

#include <cassert>

static IValue* EvaluateNegation(IValue* operand);
static IValue* EvaluateIncrement(IValue* operand);
static IValue* EvaluateDecrement(IValue* operand);
static IValue* EvaluateLogicalNot(IValue* operand);
static IValue* EvaluateTypeOf(IValue* operand);
static IValue* EvaluateToString(IValue* operand);

IValue* CRuntimeExpression::EvaluateUnary(CRuntimeContext* const ctx, const UnaryASTNode* node)
{
	auto operand = Evaluate(ctx, node->left.get());
	IValue* returnVal{ nullptr };

	if (node->IsNegation()) {
		returnVal = EvaluateNegation(operand);
	} else if (node->IsIncrement()) {
		returnVal = EvaluateIncrement(operand);
	} else if (node->IsDecrement()) {
		returnVal = EvaluateDecrement(operand);
	} else if (node->IsLogicalNot()) {
		returnVal = EvaluateLogicalNot(operand);
	} else if (node->IsTypeOf()) {
		returnVal = EvaluateTypeOf(operand);
	} else if (node->IsToString()) {
		returnVal = EvaluateToString(operand);
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

	if (operand->Type() != t_int)
		throw CRuntimeError(std::format("the increment operand must have an int type, but is \"{}\"", operand->TypeAsString()));
	
	if (!operand->HasOwner())
		throw CRuntimeError("cannot increment a temporary value");

	if (operand->IsImmutable()) 
		throw CRuntimeError("cannot increment a const value");
	

	++operand->AsInt();
	return operand;
}
IValue* EvaluateDecrement(IValue* operand)
{

	if (operand->Type() != t_int)
		throw CRuntimeError(std::format("the decrement operand must have an int type, but is \"{}\"", operand->TypeAsString()));
	
	if (!operand->HasOwner())
		throw CRuntimeError("cannot decrement a temporary value");

	if (operand->IsImmutable()) 
		throw CRuntimeError("cannot decrement a const value");
	
	--operand->AsInt();
	return operand;
}
IValue* EvaluateLogicalNot(IValue* operand)
{
	if (!operand->IsBooleanConvertible())
		throw CRuntimeError(std::format("a value of type \"{}\" is not convertible to a boolean", operand->TypeAsString()));

	return CProgramRuntime::AcquireNewValue<CBooleanValue>(!operand->ToBoolean());
}

IValue* EvaluateTypeOf(IValue* operand){
	return CStringValue::Construct(operand->TypeAsString());
}

IValue* EvaluateToString(IValue* operand) {
	return CStringValue::Construct(operand->ValueAsString());
}
