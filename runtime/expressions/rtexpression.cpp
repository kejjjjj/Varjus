#include "varjus_api/types/operators/default_operators.hpp"
#include "varjus_api/types/types.hpp"

#include "runtime/functions/rtfunction.hpp"
#include "varjus_api/internal/structure.hpp"
#include "runtime/exceptions/exception.hpp"
#include "runtime/modules/rtmodule.hpp"
#include "varjus_api/internal/runtime.hpp"
#include "varjus_api/internal/variables.hpp"

#include "linter/expressions/ast.hpp"

#include <cassert>

CRuntimeExpression::CRuntimeExpression(ASTNode&& ast) :
	m_pAST(std::move(ast)) {}
CRuntimeExpression::~CRuntimeExpression() = default;

IValue* CRuntimeExpression::Execute(CRuntimeContext* const ctx)
{
	[[maybe_unused]] const auto result = Evaluate(ctx);
	
	if (ctx->m_pRuntime->ExceptionThrown())
		return result; //we don't want the exception to get destroyed
	
	if (result && !result->HasOwner())
		result->Release();

	return nullptr;
}
IValue* CRuntimeExpression::Evaluate(CRuntimeContext* const ctx)
{
	return Evaluate(ctx, m_pAST.get());
}

#pragma pack(push)
WARNING_DISABLE(4061)
WARNING_DISABLE(4062)
IValue* CRuntimeExpression::Evaluate(CRuntimeContext* const ctx, const AbstractSyntaxTree* node)
{
	if (ctx->m_pRuntime->ExceptionThrown())
		return ctx->m_pRuntime->GetExceptionValue();

	assert(ctx->m_pRuntime->GetExceptionValue() == nullptr);
	assert(node != nullptr);
	ctx->m_pRuntime->SetExecutionPosition(&node->GetCodePosition());

	if (node->IsPostfix()) 
		return EvaluatePostfix(ctx, node->GetOperator()->GetPostfix());
	
	if (node->IsUnary())
		return EvaluateUnary(ctx, node->GetOperator()->GetUnary());

	if (node->IsSequence())
		return EvaluateSequence(ctx, node);

	if (node->IsTernary())
		return EvaluateTernary(ctx, node->GetTernary());

	if (node->IsFmtString())
		return EvaluateFmtString(ctx, node->GetFmtString());

	if (node->IsLeaf()) 
		return EvaluateLeaf(ctx, node);

	auto lhs = Evaluate(ctx, node->left.get());
	auto rhs = Evaluate(ctx, node->right.get());

	if (ctx->m_pRuntime->ExceptionThrown()) {
		if (lhs != ctx->m_pRuntime->GetExceptionValue() && !lhs->HasOwner())
			lhs->Release();
		return ctx->m_pRuntime->GetExceptionValue();
	}
	assert(node->IsOperator());

	auto& func = m_oOperatorTable[static_cast<std::size_t>(node->GetOperator()->m_ePunctuation)];

	if (!func)
		throw CRuntimeError(ctx->m_pRuntime, VSL("this operator isn't supported yet"));

	IValue* result = func(ctx->m_pRuntime, lhs, rhs);

	if (!lhs->HasOwner()) lhs->Release();
	if (!rhs->HasOwner()) rhs->Release();

	assert(result != nullptr);

	return result;
}
#pragma pack(pop)

inline IValue* EvaluateVariable(CRuntimeContext* const ctx, const VariableASTNode* const var)
{
	CVariable* variable{ nullptr };

	if (var->m_bGlobalVariable) {
		auto activeModule = var->m_bBelongsToDifferentModule
			? ctx->m_pRuntime->GetModuleByIndex(var->m_uModuleIndex)
			: ctx->m_pModule;

		assert(activeModule);
		variable = activeModule->GetGlobalVariableByIndex(var->m_uIndex);
	} else {
		assert(ctx->m_pFunction);
		variable = ctx->m_pFunction->GetVariableByRef(*var);
	}

	assert(variable->GetValue() && variable->GetValue()->HasOwner());

	if (var->m_bSelfCapturing) {
		variable->m_bSelfCapturing = true;
	}
	auto& v = variable->GetValue();
	//if (var->m_bIsConst) // this was already validated during linting
	//	v->MakeImmutable();

	return v;
}
inline IValue* EvaluateFunction(CRuntimeContext* const ctx, const FunctionASTNode* const var)
{
	auto activeModule = var->m_bBelongsToDifferentModule
		? ctx->m_pRuntime->GetModuleByIndex(var->m_uModuleIndex)
		: ctx->m_pModule;

	auto v = CCallableValue::Construct(ctx->m_pRuntime, activeModule->GetFunctionByIndex(var->m_uIndex));

	if (var->m_bBelongsToDifferentModule)
		v->Internal()->SetModuleIndex(var->m_uModuleIndex);

	v->MakeImmutable();
	return v;
}
inline IValue* EvaluateLambda(CRuntimeContext* const ctx, const LambdaASTNode* const var)
{
	auto v = CCallableValue::Construct(ctx->m_pRuntime, var->m_pLambda.get());

	if (var->m_oVariableCaptures.size())
		v->Internal()->SetCaptures(ctx, var->m_oVariableCaptures);

	return v;
}
IValue* CRuntimeExpression::EvaluateLeaf(CRuntimeContext* const ctx, const AbstractSyntaxTree* node)
{

	if (node->IsVariable()) {
		return EvaluateVariable(ctx, node->GetVariable());
	}

	if (node->IsFunction()) {
		return EvaluateFunction(ctx, node->GetFunction());
	}

	if (node->IsLambda()) {
		return EvaluateLambda(ctx, node->GetLambda());
	}

	if (node->IsArray()) {
		return CArrayValue::Construct(ctx->m_pRuntime, EvaluateList(ctx, node->GetArray()->m_oExpressions));
	}

	if (node->IsObject()) {
		return CObjectValue::_ConstructInternal(ctx->m_pRuntime, EvaluateObject(ctx, node->GetObject()->m_oAttributes));
	}

	if (node->IsConstant()) {
		const auto constant = node->GetConstant();
		switch (constant->m_eDataType) {
			case t_undefined:
				return IValue::Construct(ctx->m_pRuntime);
			case t_boolean:
				return CBooleanValue::Construct(ctx->m_pRuntime, static_cast<bool>(constant->m_pConstant[0]));
			case t_int:
				return CIntValue::Construct(ctx->m_pRuntime, *reinterpret_cast<VarjusInt*>((char*)constant->m_pConstant.data()));
			case t_uint:
				return CUIntValue::Construct(ctx->m_pRuntime, *reinterpret_cast<VarjusUInt*>((char*)constant->m_pConstant.data()));
			case t_double:
				return CDoubleValue::Construct(ctx->m_pRuntime, *reinterpret_cast<VarjusDouble*>((char*)constant->m_pConstant.data()));
			case t_string:
				return CStringValue::Construct(ctx->m_pRuntime, constant->m_pConstant);
			default:
				break;
		}
	}

	assert(false);
	return nullptr;
}
IValue* CRuntimeExpression::EvaluateSequence(CRuntimeContext* const ctx, const AbstractSyntaxTree* node)
{
	//discard lhs
	auto lhs = Evaluate(ctx, node->left.get());

	if (!lhs->HasOwner())
		lhs->Release();

	return Evaluate(ctx, node->right.get());
}

IValue* CRuntimeExpression::EvaluateTernary(CRuntimeContext* const ctx, const TernaryASTNode* node)
{
	auto operand = Evaluate(ctx, node->m_pOperand.get());

	if (!operand->IsBooleanConvertible())
		throw CRuntimeError(ctx->m_pRuntime, VSL("the operand is not convertible to a boolean"));

	const auto boolValue = operand->ToBoolean();

	if (!operand->HasOwner())
		operand->Release();

	if (boolValue) {
		return Evaluate(ctx, node->m_pTrue.get());
	}

	return Evaluate(ctx, node->m_pFalse.get());

}
IValue* CRuntimeExpression::EvaluateFmtString(CRuntimeContext* const ctx, const FmtStringASTNode* node)
{
	VarjusString fullString;
	for (const auto& [t, v] : node->m_oNodes) {

		if (t == FmtStringAST::TEXT) {
			fullString += std::get<0>(v);
		}else if (t == FmtStringAST::PLACEHOLDER) {

			auto operand = Evaluate(ctx, std::get<1>(v).get());
			fullString += operand->ValueAsString();

			if (!operand->HasOwner())
				operand->Release();

		}

	}

	return CStringValue::Construct(ctx->m_pRuntime, fullString);
}
