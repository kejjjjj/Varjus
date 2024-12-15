#include "runtime/structure.hpp"
#include "runtime/functions/rtfunction.hpp"
#include "runtime/values/types/types.hpp"
#include "runtime/values/simple_operators.hpp"

#include "runtime/variables.hpp"
#include "runtime/runtime.hpp"
#include "runtime/exceptions/exception.hpp"
#include "runtime/modules/rtmodule.hpp"

#include "linter/expressions/ast.hpp"

#include <cassert>

CRuntimeExpression::CRuntimeExpression(std::unique_ptr<AbstractSyntaxTree>&& ast) :
	m_pAST(std::move(ast)) {}
CRuntimeExpression::~CRuntimeExpression() = default;

IValue* CRuntimeExpression::Execute(CRuntimeContext* const ctx)
{
	//assert(m_pAST);
	//CProgramRuntime::SetExecutionPosition(&m_pAST->GetCodePosition());

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

IValue* CRuntimeExpression::EvaluateLeaf(CRuntimeContext* const ctx, const AbstractSyntaxTree* node)
{



	if (node->IsVariable()) {
		const auto var = node->GetVariable();

		IValue* v = nullptr;

		if (var->m_bGlobalVariable) {
			v = ctx->m_pModule->GetGlobalVariableByIndex(var->m_uIndex)->GetValue();
		} else {
			assert(ctx->m_pFunction);
			v = ctx->m_pFunction->GetVariableByIndex(var->m_uIndex)->GetValue();
		}

		assert(v);
		assert(v->HasOwner());
		return v;
	}

	if (node->IsFunction()) {
		const auto var = node->GetFunction();
		auto v = CProgramRuntime::AcquireNewValue<CCallableValue>();
		v->MakeShared();
		v->Internal()->GetCallable() = ctx->m_pModule->GetFunctionByIndex(var->m_uIndex);
		v->MakeImmutable();
		return v;
	}

	if (node->IsLambda()) {
		const auto var = node->GetLambda();
		auto v = CProgramRuntime::AcquireNewValue<CCallableValue>();
		v->MakeShared();

		auto internal = v->Internal();
		internal->GetCallable() = var->m_pLambda.get();
		if (var->m_oVariableCaptures.size())
			internal->SetCaptures(ctx, var->m_oVariableCaptures);

		return v;
	}

	if (node->IsArray()) {
		return CArrayValue::Construct(EvaluateList(ctx, node->GetArray()->m_oExpressions));
	}

	if (node->IsObject()) {
		return CObjectValue::Construct(EvaluateObject(ctx, node->GetObject()->m_oAttributes));
	}

	if (node->IsConstant()) {
		const auto constant = node->GetConstant();
		switch (constant->m_eDataType) {
			case t_undefined:
				return CProgramRuntime::AcquireNewValue<IValue>();
			case t_boolean:
				return CProgramRuntime::AcquireNewValue<CBooleanValue>(static_cast<bool>(constant->m_pConstant[0]));
			case t_int:
				return CProgramRuntime::AcquireNewValue<CIntValue>(*reinterpret_cast<std::int64_t*>((char*)constant->m_pConstant.data()));
			case t_double:
				return CProgramRuntime::AcquireNewValue<CDoubleValue>(*reinterpret_cast<double*>((char*)constant->m_pConstant.data()));
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
