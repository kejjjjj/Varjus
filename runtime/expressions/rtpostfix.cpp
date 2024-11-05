#include "rtpostfix.hpp"
#include "rtexpression.hpp"

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

IValue* CRuntimeExpression::EvaluatePostfix(CFunction* const thisFunction, const OperatorASTNode* node)
{
	auto operand = Evaluate(thisFunction, node->left.get());

	if (node->IsSubscript()) 
		return EvaluateSubscript(thisFunction, operand, node->As<const SubscriptASTNode*>());
	
	if(node->IsFunctionCall())
		return EvaluateFunctionCall(thisFunction, operand, node->As<const FunctionCallASTNode*>());

	return nullptr;
}

IValue* CRuntimeExpression::EvaluateSubscript(CFunction* const thisFunction, IValue* operand, const SubscriptASTNode* node)
{
	if (!operand->IsIndexable())
		throw CRuntimeError(std::format("a value of type \"{}\" cannot be indexed", operand->TypeAsString()));

	auto accessor = Evaluate(thisFunction, node->m_pAST.get());

	if (!accessor->IsIntegral())
		throw CRuntimeError(std::format("array accessor must be integral, but is \"{}\"", accessor->TypeAsString()));

	if (!accessor->HasOwner())
		accessor->Release();

	auto index = operand->Index(accessor->ToInt());

	index->MakeImmutable(); //cannot modify parts

	if (!operand->HasOwner())
		operand->Release();

	return index;
}
IValue* CRuntimeExpression::EvaluateFunctionCall(
	[[maybe_unused]]CFunction* const thisFunction, [[maybe_unused]] IValue* operand, [[maybe_unused]] const FunctionCallASTNode* node)
{


	return nullptr;
}
