#include "varjus_api/types/operators/default_operators.hpp"
#include "varjus_api/types/types.hpp"

#include "runtime/functions/rtfunction.hpp"
#include "varjus_api/internal/structure.hpp"
#include "runtime/exceptions/exception.hpp"
#include "varjus_api/internal/runtime.hpp"
#include "varjus_api/internal/variables.hpp"

#include "linter/expressions/ast.hpp"

#include <cassert>


static IValue* EvaluateIncrement(CProgramRuntime* const runtime, IValue* operand);
static IValue* EvaluateDecrement(CProgramRuntime* const runtime, IValue* operand);

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
		assert(operand && operand->GetAllocator());
		returnVal = EvaluateFunctionCall(ctx, operand, node->GetFunctionCall());
		assert(operand && operand->GetAllocator());
	} else if (node->IsMemberAccess()) {
		returnVal = EvaluateMemberAccess(ctx, operand, node->GetMemberAccess());

		if (operand->IsHanging() && operand->Type() == t_object) {
			if(returnVal->HasOwner())
				returnVal = returnVal->Copy(); //accessing a temporary e.g. {a: 50, b: 1, c: 2}.a
		}

	} else if (node->IsIncrement()) {
		returnVal = EvaluateIncrement(ctx->m_pRuntime, operand);
	} else if (node->IsDecrement()) {
		returnVal = EvaluateDecrement(ctx->m_pRuntime, operand);
	}

	if (!operand->HasOwner()) {
		operand->Release();
	}
	
	assert(returnVal);
	return returnVal;
}
IValue* CRuntimeExpression::EvaluateMemberAccess(CRuntimeContext* const ctx, IValue* operand, const MemberAccessASTNode* node)
{

	if(!operand->IsAggregate())
		throw CRuntimeError(operand->GetAllocator(), fmt::format(VSL("a value of type \"{}\" is not aggregate"), operand->TypeAsString()));

	return operand->GetAggregate(ctx, node->m_uGlobalMemberIndex);

}
IValue* CRuntimeExpression::EvaluateSubscript(CRuntimeContext* const ctx, IValue* operand, const SubscriptASTNode* node)
{
	if (!operand->IsIndexable())
		throw CRuntimeError(ctx->m_pRuntime, fmt::format(VSL("a value of type \"{}\" cannot be indexed"), operand->TypeAsString()));

	auto accessor = Evaluate(ctx, node->m_pAST.get());

	auto index = operand->Index(accessor);

	if (!accessor->HasOwner())
		accessor->Release();

	assert(index);

	return index;
}
IValue* CRuntimeExpression::EvaluateFunctionCall(CRuntimeContext* const ctx, IValue* operand, const FunctionCallASTNode* node)
{

	if (!operand->IsCallable())
		throw CRuntimeError(ctx->m_pRuntime, fmt::format(VSL("a value of type \"{}\" is not callable"), operand->TypeAsString()));

	// the callee will take ownership of temp-value args
	auto args = EvaluateList(ctx, node->m_oArguments);
	return operand->Call(ctx, args);
}

IValue* EvaluateIncrement(CProgramRuntime* const runtime, IValue* operand)
{
	
	if (operand->Type() != t_int && operand->Type() != t_uint)
		throw CRuntimeError(runtime, fmt::format(VSL("the increment operand must have an (u)int type, but is \"{}\""), operand->TypeAsString()));
	
	if (!operand->HasOwner())
		throw CRuntimeError(runtime, VSL("cannot increment a temporary value"));

	if (operand->IsImmutable()) 
		throw CRuntimeError(runtime, VSL("cannot increment a const value"));
	
	if (operand->Type() == t_int) {
		auto v = CIntValue::Construct(runtime, operand->AsInt()); //create temp old value
		++operand->AsInt(); //but increment this value
		return v;
	} 
	
	auto v = CUIntValue::Construct(runtime, operand->AsUInt()); //create temp old value
	++operand->AsUInt(); //but increment this value
	return v;
}
IValue* EvaluateDecrement(CProgramRuntime* const runtime, IValue* operand)
{
	if (operand->Type() != t_int && operand->Type() != t_uint)
		throw CRuntimeError(runtime, fmt::format(VSL("the decrement operand must have an (u)int type, but is \"{}\""), operand->TypeAsString()));

	if (!operand->HasOwner())
		throw CRuntimeError(runtime, VSL("cannot decrement a temporary value"));

	if (operand->IsImmutable()) 
		throw CRuntimeError(runtime, VSL("cannot decrement a const value"));
	
	if (operand->Type() == t_int) {
		auto v = CIntValue::Construct(runtime, operand->AsInt()); //create temp old value
		--operand->AsInt(); //but decrement this value
		return v;
	}

	auto v = CUIntValue::Construct(runtime, operand->AsUInt()); //create temp old value
	--operand->AsUInt(); //but decrement this value
	return v;
}