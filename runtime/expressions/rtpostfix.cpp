#include "runtime/structure.hpp"
#include "runtime/functions/rtfunction.hpp"
#include "runtime/values/types/types.hpp"

#include "runtime/values/simple_operators.hpp"

#include "runtime/variables.hpp"
#include "runtime/runtime.hpp"
#include "runtime/exceptions/exception.hpp"

#include "linter/expressions/ast.hpp"

#include <cassert>
#include <format>

static IValue* EvaluateIncrement(IValue* operand);
static IValue* EvaluateDecrement(IValue* operand);

IValue* CRuntimeExpression::EvaluatePostfix(CRuntimeContext* const ctx, const PostfixASTNode* node)
{
	auto operand = Evaluate(ctx, node->left.get());

	IValue* returnVal{ nullptr };

	if (node->IsSubscript()) {
		
		returnVal = EvaluateSubscript(ctx, operand, node->GetSubscript());

		if (operand->IsHanging() && operand->IsSharedObject()) {
			returnVal = returnVal->Copy(); //accessing a temporary e.g. [[1, 2, 3]][0]
		}

	} else if (node->IsFunctionCall()) {
		returnVal = EvaluateFunctionCall(ctx, operand, node->GetFunctionCall());
	} else if (node->IsMemberAccess()) {
		returnVal = EvaluateMemberAccess(operand, node->GetMemberAccess());

		const auto isMethod = !returnVal->IsSharedObject() && returnVal->IsCallable();

		if (!isMethod && operand->IsHanging()) {
			returnVal = returnVal->Copy(); //accessing a temporary e.g. [1, 2, 3].length
		}

	} else if (node->IsIncrement()) {
		returnVal = EvaluateIncrement(operand);
	} else if (node->IsDecrement()) {
		returnVal = EvaluateDecrement(operand);
	}

	if (!operand->HasOwner()) 
		operand->Release();
	
	assert(returnVal);
	return returnVal;
}
IValue* CRuntimeExpression::EvaluateMemberAccess(IValue* operand, const MemberAccessASTNode* node)
{

	if(!operand->IsAggregate())
		throw CRuntimeError(std::format("a value of type \"{}\" is not aggregate", operand->TypeAsString()));

	return operand->GetAggregate(node->m_uGlobalMemberIndex);

}
IValue* CRuntimeExpression::EvaluateSubscript(CRuntimeContext* const ctx, IValue* operand, const SubscriptASTNode* node)
{
	if (!operand->IsIndexable())
		throw CRuntimeError(std::format("a value of type \"{}\" cannot be indexed", operand->TypeAsString()));

	auto accessor = Evaluate(ctx, node->m_pAST.get());

	if (!accessor->IsIntegral())
		throw CRuntimeError(std::format("array accessor must be integral, but is \"{}\"", accessor->TypeAsString()));

	auto index = operand->Index(accessor->ToInt());

	if (!accessor->HasOwner())
		accessor->Release();

	assert(index);

	return index;
}
IValue* CRuntimeExpression::EvaluateFunctionCall(CRuntimeContext* const ctx, IValue* operand, const FunctionCallASTNode* node)
{

	if (!operand->IsCallable())
		throw CRuntimeError(std::format("a value of type \"{}\" is not callable", operand->TypeAsString()));

	// the callee will take ownership of temp-value args
	auto args = EvaluateList(ctx, node->m_oArguments);
	return operand->Call(ctx, args);
}

IValue* EvaluateIncrement(IValue* operand)
{

	if (operand->Type() != t_int)
		throw CRuntimeError(std::format("the increment operand must have an int type, but is \"{}\"", operand->TypeAsString()));
	
	if (!operand->HasOwner())
		throw CRuntimeError("cannot increment a temporary value");

	auto v = CProgramRuntime::AcquireNewValue<CIntValue>(operand->AsInt()); //create temp old value
	++operand->AsInt(); //but increment this value

	return v;
}
IValue* EvaluateDecrement(IValue* operand)
{

	if (operand->Type() != t_int)
		throw CRuntimeError(std::format("the decrement operand must have an int type, but is \"{}\"", operand->TypeAsString()));

	if (!operand->HasOwner())
		throw CRuntimeError("cannot decrement a temporary value");

	auto v = CProgramRuntime::AcquireNewValue<CIntValue>(operand->AsInt()); //create temp old value
	--operand->AsInt(); //but decrement this value
	return v;
}