#include "varjus_api/types/operators/default_operators.hpp"
#include "varjus_api/types/types.hpp"

#include "runtime/functions/rtfunction.hpp"
#include "varjus_api/internal/structure.hpp"
#include "varjus_api/internal/exceptions/exception.hpp"
#include "varjus_api/internal/runtime.hpp"
#include "varjus_api/internal/variables.hpp"

#include "linter/expressions/ast.hpp"

#include <cassert>

static IValue* EvaluateNegation(CProgramRuntime* const runtime, IValue* operand);
static IValue* EvaluateIncrement(IValue* operand);
static IValue* EvaluateDecrement(IValue* operand);
static IValue* EvaluateLogicalNot(CProgramRuntime* const runtime, IValue* operand);
static IValue* EvaluateBitwiseNot(CProgramRuntime* const runtime, IValue* operand);

static IValue* EvaluateTypeOf(CProgramRuntime* const runtime, IValue* operand);
static IValue* EvaluateToString(CProgramRuntime* const runtime, IValue* operand);

IValue* CRuntimeExpression::EvaluateUnary(CRuntimeContext* const ctx, UnaryASTNode* node)
{
	auto operand = Evaluate(ctx, node->left);
	IValue* returnVal{ nullptr };

	if (node->IsNegation()) {
		returnVal = EvaluateNegation(ctx->m_pRuntime, operand);
	} else if (node->IsIncrement()) {
		returnVal = EvaluateIncrement(operand);
	} else if (node->IsDecrement()) {
		returnVal = EvaluateDecrement(operand);
	} else if (node->IsLogicalNot()) {
		returnVal = EvaluateLogicalNot(ctx->m_pRuntime, operand);
	} else if (node->IsBitwiseNot()) {
		returnVal = EvaluateBitwiseNot(ctx->m_pRuntime, operand);
	} else if (node->IsTypeOf()) {
		returnVal = EvaluateTypeOf(ctx->m_pRuntime, operand);
	} else if (node->IsToString()) {
		returnVal = EvaluateToString(ctx->m_pRuntime, operand);
	}
	
	assert(returnVal);

	if (!operand->HasOwner())
		operand->Release();

	return returnVal;
}
IValue* EvaluateNegation(CProgramRuntime* const runtime, IValue* operand)
{
	if (operand->Type() == t_int) {
		return CIntValue::Construct(runtime, -operand->AsInt());
	}

	if (operand->Type() == t_double) {
		return CDoubleValue::Construct(runtime, -operand->AsDouble());
	}

	throw CRuntimeError(runtime, fmt::format(VSL("cannot negate a value of type \"{}\""), operand->TypeAsString()));
}
IValue* EvaluateIncrement(IValue* operand)
{

	if (operand->Type() != t_int && operand->Type() != t_uint)
		throw CRuntimeError(operand->GetAllocator(), fmt::format(VSL("the increment operand must have an (u)int type, but is \"{}\""), operand->TypeAsString()));
	
	if (!operand->HasOwner())
		throw CRuntimeError(operand->GetAllocator(), VSL("cannot increment a temporary value"));

	if (operand->IsImmutable()) 
		throw CRuntimeError(operand->GetAllocator(), VSL("cannot increment a const value"));
	
	if(operand->Type() == t_int)
		++operand->AsInt();
	else
		++operand->AsUInt();

	return operand;
}
IValue* EvaluateDecrement(IValue* operand)
{

	if (operand->Type() != t_int && operand->Type() != t_uint)
		throw CRuntimeError(operand->GetAllocator(), fmt::format(VSL("the decrement operand must have an (u)int type, but is \"{}\""), operand->TypeAsString()));
	
	if (!operand->HasOwner())
		throw CRuntimeError(operand->GetAllocator(), VSL("cannot decrement a temporary value"));

	if (operand->IsImmutable()) 
		throw CRuntimeError(operand->GetAllocator(), VSL("cannot decrement a const value"));
	
	if (operand->Type() == t_int)
		--operand->AsInt();
	else
		--operand->AsUInt();

	return operand;
}
IValue* EvaluateLogicalNot(CProgramRuntime* const runtime, IValue* operand)
{
	if (!operand->IsBooleanConvertible())
		throw CRuntimeError(runtime, fmt::format(VSL("a value of type \"{}\" is not convertible to a boolean"), operand->TypeAsString()));

	return CBooleanValue::Construct(runtime, !operand->ToBoolean());
}
IValue* EvaluateBitwiseNot(CProgramRuntime* const runtime, IValue* operand)
{

	if (operand->Type() != t_int && operand->Type() != t_uint)
		throw CRuntimeError(runtime, fmt::format(VSL("the bitwise-not operand must have an (u)int type, but is \"{}\""), operand->TypeAsString()));

	if (operand->Type() == t_int)
		return CIntValue::Construct(runtime, ~operand->AsInt());

	return CUIntValue::Construct(runtime, ~operand->AsUInt());
}
IValue* EvaluateTypeOf(CProgramRuntime* const runtime, IValue* operand){
	return CStringValue::Construct(runtime, operand->TypeAsString());
}

IValue* EvaluateToString(CProgramRuntime* const runtime, IValue* operand) {
	return CStringValue::Construct(runtime, operand->ValueAsEscapedString());
}
