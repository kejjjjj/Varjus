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
		return EvaluatePostfix(thisFunction, node->As<const OperatorASTNode*>());
	
	if (node->IsSequence())
		return EvaluateSequence(thisFunction, node);

	if (node->IsLeaf()) 
		return EvaluateLeaf(thisFunction, node);
	

	auto lhs = Evaluate(thisFunction, node->left.get());
	auto rhs = Evaluate(thisFunction, node->right.get());

	assert(node->IsOperator());

	IValue* result{ nullptr };

	switch (node->As<const OperatorASTNode*>()->m_ePunctuation) {
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

	return result;
}
#pragma pack(pop)

IValue* CRuntimeExpression::EvaluateLeaf(CFunction* const thisFunction, const AbstractSyntaxTree* node)
{
	if (node->IsVariable()) {
		const auto var = node->As<const VariableASTNode*>();
		return thisFunction->GetVariableByIndex(var->m_uIndex)->GetValue();
	}

	if (node->IsFunction()) {
		const auto var = node->As<const FunctionASTNode*>();
		auto v = CProgramRuntime::AcquireNewValue<CCallableValue>(CProgramRuntime::GetFunctionByIndex(var->m_uIndex));
		v->MakeImmutable();
		return v;
	}

	if (node->IsConstant()) {
		const auto constant = node->As<const ConstantASTNode*>();
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
				return CProgramRuntime::AcquireNewValue<CStringValue>(constant->m_pConstant);
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