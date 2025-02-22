#include "api/types/operators/default_operators.hpp"
#include "api/types/types.hpp"

#include "runtime/functions/rtfunction.hpp"
#include "api/internal/structure.hpp"
#include "runtime/exceptions/exception.hpp"
#include "runtime/modules/rtmodule.hpp"
#include "api/internal/runtime.hpp"
#include "api/internal/variables.hpp"

#include "linter/expressions/ast.hpp"

#include <cassert>

CRuntimeExpression::CRuntimeExpression(ASTNode&& ast) :
	m_pAST(std::move(ast)) {}
CRuntimeExpression::~CRuntimeExpression() = default;

IValue* CRuntimeExpression::Execute(CRuntimeContext* const ctx)
{
	[[maybe_unused]] const auto result = Evaluate(ctx);
	
	if (CProgramRuntime::ExceptionThrown())
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
#pragma warning(disable : 4061)
#pragma warning(disable : 4062)
IValue* CRuntimeExpression::Evaluate(CRuntimeContext* const ctx, const AbstractSyntaxTree* node)
{
	if (CProgramRuntime::ExceptionThrown())
		return CProgramRuntime::GetExceptionValue();

	assert(node != nullptr);
	CProgramRuntime::SetExecutionPosition(&node->GetCodePosition());

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

	assert(node->IsOperator());

	auto& func = m_oOperatorTable[static_cast<std::size_t>(node->GetOperator()->m_ePunctuation)];

	if (!func)
		throw CRuntimeError("this operator isn't supported yet");

	IValue* result = func(lhs, rhs);

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
			? CProgramRuntime::GetModuleByIndex(var->m_uModuleIndex)
			: ctx->m_pModule;

		assert(activeModule);
		variable = activeModule->GetGlobalVariableByIndex(var->m_uIndex);
	} else {
		assert(ctx->m_pFunction);
		variable = ctx->m_pFunction->GetVariableByRef(*var);
	}

	assert(variable->GetValue() && variable->GetValue()->HasOwner());

	if (var->m_bSelfCapturing)
		variable->m_bSelfCapturing = true;

	auto& v = variable->GetValue();
	if (var->m_bIsConst)
		v->MakeImmutable();

	return v;
}
inline IValue* EvaluateFunction(CRuntimeContext* const ctx, const FunctionASTNode* const var)
{
	auto activeModule = var->m_bBelongsToDifferentModule
		? CProgramRuntime::GetModuleByIndex(var->m_uModuleIndex)
		: ctx->m_pModule;

	auto v = CCallableValue::Construct(activeModule->GetFunctionByIndex(var->m_uIndex));

	if (var->m_bBelongsToDifferentModule)
		v->Internal()->SetModuleIndex(var->m_uModuleIndex);

	v->MakeImmutable();
	return v;
}
inline IValue* EvaluateLambda(CRuntimeContext* const ctx, const LambdaASTNode* const var)
{
	auto v = CCallableValue::Construct(var->m_pLambda.get());

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
		return CArrayValue::Construct(EvaluateList(ctx, node->GetArray()->m_oExpressions));
	}

	if (node->IsObject()) {
		return CObjectValue::Construct(ctx->m_pModule->GetIndex() ,EvaluateObject(ctx, node->GetObject()->m_oAttributes));
	}

	if (node->IsConstant()) {
		const auto constant = node->GetConstant();
		switch (constant->m_eDataType) {
			case t_undefined:
				return IValue::Construct();
			case t_boolean:
				return CBooleanValue::Construct(static_cast<bool>(constant->m_pConstant[0]));
			case t_int:
				return CIntValue::Construct(*reinterpret_cast<VarjusInt*>((char*)constant->m_pConstant.data()));
			case t_uint:
				return CUIntValue::Construct(*reinterpret_cast<VarjusUInt*>((char*)constant->m_pConstant.data()));
			case t_double:
				return CDoubleValue::Construct(*reinterpret_cast<VarjusDouble*>((char*)constant->m_pConstant.data()));
			case t_string:
				return CStringValue::Construct(constant->m_pConstant);
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
		throw CRuntimeError("the operand is not convertible to a boolean");

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
	std::string fullString;
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

	return CStringValue::Construct(fullString);
}
