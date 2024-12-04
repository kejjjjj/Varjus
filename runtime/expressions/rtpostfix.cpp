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

IValue* CRuntimeExpression::EvaluatePostfix(CFunction* const thisFunction, const PostfixASTNode* node)
{
	auto operand = Evaluate(thisFunction, node->left.get());

	IValue* returnVal{ nullptr };

	if (node->IsSubscript()) {
		returnVal = EvaluateSubscript(thisFunction, operand, node->As<const SubscriptASTNode*>());
		
		if (operand->IsHanging()) {
			//accessing a temporary e.g. [[1, 2, 3]][0]
			returnVal = returnVal->Copy();
		}

	} else if (node->IsFunctionCall()) {
		returnVal = EvaluateFunctionCall(thisFunction, operand, node->As<const FunctionCallASTNode*>());
	} else if (node->IsMemberAccess()) {
		returnVal = EvaluateMemberAccess(operand, node->As<const MemberAccessASTNode*>());

		if (operand->IsHanging()) {
			//accessing a temporary e.g. [1, 2, 3].length
			returnVal = returnVal->Copy();
		}
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
IValue* CRuntimeExpression::EvaluateSubscript(CFunction* const thisFunction, IValue* operand, const SubscriptASTNode* node)
{
	if (!operand->IsIndexable())
		throw CRuntimeError(std::format("a value of type \"{}\" cannot be indexed", operand->TypeAsString()));

	auto accessor = Evaluate(thisFunction, node->m_pAST.get());

	if (!accessor->IsIntegral())
		throw CRuntimeError(std::format("array accessor must be integral, but is \"{}\"", accessor->TypeAsString()));

	auto index = operand->Index(accessor->ToInt());

	if (!accessor->HasOwner())
		accessor->Release();

	assert(index);

	return index;
}
IValue* CRuntimeExpression::EvaluateFunctionCall(CFunction* const thisFunction, IValue* operand, const FunctionCallASTNode* node)
{

	if (!operand->IsCallable())
		throw CRuntimeError(std::format("a value of type \"{}\" is not callable", operand->TypeAsString()));

	// the callee will take ownership of temp-value args
	auto args = EvaluateList(thisFunction, node->m_oArguments);

	return operand->Call(thisFunction, args);

	//if (operand->IsBuiltInMemberCallable()) {

	//	auto callable = operand->ToBuiltInMemberCallable();
	//	auto& mem_fn = callable->Get();

	//	return CStaticArrayBuiltInMethods::CallMethod(mem_fn.m_pThis->ToArray(), args, mem_fn.m_pMethod);
	//}

	//auto callable = operand->ToCallable()->Internal();
	//auto function = callable->GetCallable();
	////std::cout << "calling: " << function->GetName() << " with " << callable->GetCaptures().size() << '\n';

	//auto ret = function->Execute(thisFunction, args, callable->GetCaptures());

	//assert(ret);

	//return ret;
}
