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

CRuntimeExpression::CRuntimeExpression(std::unique_ptr<AbstractSyntaxTree>&& ast) :
	m_pAST(std::move(ast)) {}
CRuntimeExpression::~CRuntimeExpression() = default;

IValue* CRuntimeExpression::Execute([[maybe_unused]]CFunction* const thisFunction)
{

	[[maybe_unused]] const auto result = Evaluate(thisFunction);
	
	if (result && !result->HasOwner())
		result->Release();
	//TODO: make sure that all objects from the pool get released

	return nullptr;
}
IValue* CRuntimeExpression::Evaluate(CFunction* const thisFunction)
{
	return Evaluate(thisFunction, m_pAST.get());
}

#pragma pack(push)
#pragma warning(disable : 4061)
#pragma warning(disable : 4062)
IValue* CRuntimeExpression::Evaluate(CFunction* const thisFunction, const AbstractSyntaxTree* node)
{

	assert(node != nullptr);

	if (node->IsPostfix()) 
		return EvaluatePostfix(thisFunction, node->GetOperator());
	
	if (node->IsSequence())
		return EvaluateSequence(thisFunction, node);

	if (node->IsTernary())
		return EvaluateTernary(thisFunction, node->GetTernary());

	if (node->IsLeaf()) 
		return EvaluateLeaf(thisFunction, node);
	

	auto lhs = Evaluate(thisFunction, node->left.get());
	auto rhs = Evaluate(thisFunction, node->right.get());

	assert(node->IsOperator());

	IValue* result{ nullptr };

	switch (node->GetOperator()->m_ePunctuation) {
	case p_assign:
		result = OP_ASSIGNMENT(lhs, rhs);
		break;
	case p_add:
		result = OP_ADDITION(lhs, rhs);
		break;
	case p_less_than:
		result = OP_LESS_THAN(lhs, rhs);
		break;
	default:
		throw std::exception("bad operator");
	}

	if (!lhs->HasOwner()) lhs->Release();
	if (!rhs->HasOwner()) rhs->Release();

	assert(result != nullptr);

	return result;
}
#pragma pack(pop)

IValue* CRuntimeExpression::EvaluateLeaf(CFunction* const thisFunction, const AbstractSyntaxTree* node)
{

	if (node->IsVariable()) {
		const auto var = node->GetVariable();
		auto v = thisFunction->GetVariableByIndex(var->m_uIndex)->GetValue();
		assert(v);
		assert(v->HasOwner());
		return v;
	}

	if (node->IsFunction()) {
		const auto var = node->GetFunction();
		auto v = CProgramRuntime::AcquireNewValue<CCallableValue>();
		v->MakeShared();

		v->Internal()->GetCallable() = CProgramRuntime::GetFunctionByIndex(var->m_uIndex);

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
			internal->SetCaptures(thisFunction, var->m_oVariableCaptures);

		return v;
	}

	if (node->IsArray()) {
		const auto var = node->GetArray();
		auto ptr = CProgramRuntime::AcquireNewValue<CArrayValue>();
		ptr->MakeShared();
		auto internal = ptr->Internal();
		internal->Set(EvaluateList(thisFunction, var->m_oExpressions));
		internal->GetAggregateValue().Setup(runtime::__internal::GetAggregateArrayData());
		return ptr;
	}

	if (node->IsObject()) {
		auto ptr = CProgramRuntime::AcquireNewValue<CObjectValue>();
		ptr->MakeShared();
		auto internal = ptr->Internal();
		internal->Set(EvaluateObject(thisFunction, node->GetObject()->m_oAttributes));
		return ptr;
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
IValue* CRuntimeExpression::EvaluateSequence(CFunction* const thisFunction, const AbstractSyntaxTree* node)
{
	//discard lhs
	auto lhs = Evaluate(thisFunction, node->left.get());

	if (!lhs->HasOwner())
		lhs->Release();

	return Evaluate(thisFunction, node->right.get());
}

IValue* CRuntimeExpression::EvaluateTernary(CFunction* const thisFunction, const TernaryASTNode* node)
{
	auto operand = Evaluate(thisFunction, node->m_pOperand.get());

	if (!operand->IsBooleanConvertible())
		throw CRuntimeError("the operand is not convertible to a boolean");

	const auto boolValue = operand->ToBoolean();

	if (!operand->HasOwner())
		operand->Release();

	if (boolValue) {
		return Evaluate(thisFunction, node->m_pTrue.get());
	}

	return Evaluate(thisFunction, node->m_pFalse.get());

}
