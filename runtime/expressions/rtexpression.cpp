#include "runtime/structure.hpp"
#include "runtime/functions/rtfunction.hpp"
#include "runtime/values/types/types.hpp"
#include "runtime/values/simple_operators.hpp"

#include "runtime/variables.hpp"
#include "runtime/runtime.hpp"

#include "linter/expressions/ast.hpp"

#include <cassert>

CRuntimeExpression::CRuntimeExpression(std::unique_ptr<AbstractSyntaxTree>&& ast) :
	m_pAST(std::move(ast)) {}
CRuntimeExpression::~CRuntimeExpression() = default;

bool CRuntimeExpression::Execute([[maybe_unused]]CFunction* const thisFunction)
{

	[[maybe_unused]] const auto result = Evaluate(thisFunction);
	
	if (!result->HasOwner())
		result->Release();

	//TODO: make sure that all objects from the pool get released

	return false;
}
IValue* CRuntimeExpression::Evaluate(CFunction* const thisFunction)
{
	return Evaluate(thisFunction, m_pAST.get());
}

#pragma pack(push)
#pragma warning(disable : 4061)
#pragma warning(disable : 4062)
IValue* CRuntimeExpression::Evaluate(CFunction* const thisFunction, AbstractSyntaxTree* node)
{

	assert(node != nullptr);

	if (node->IsLeaf()) {
		return EvaluateLeaf(thisFunction, node);
	}

	auto lhs = Evaluate(thisFunction, node->left.get());
	auto rhs = Evaluate(thisFunction, node->right.get());

	assert(node->IsOperator());

	IValue* result{ nullptr };

	switch (node->As<const OperatorASTNode*>()->m_ePunctuation) {
	case p_assign:
		result = lhs;
		OP_ASSIGNMENT(lhs, rhs);
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

IValue* CRuntimeExpression::EvaluateLeaf(CFunction* const thisFunction, AbstractSyntaxTree* node)
{
	if (node->IsVariable()) {
		const auto var = node->As<const VariableASTNode*>();
		return thisFunction->GetVariableByIndex(var->m_uIndex)->GetValue();
	}

	if (node->IsConstant()) {
		const auto constant = node->As<const ConstantASTNode*>();
		switch (constant->m_eDataType) {
			case t_undefined:
				return CProgramRuntime::AcquireNewValue();
			case t_boolean:
				return CProgramRuntime::AcquireNewBooleanValue(static_cast<bool>(constant->m_pConstant[0]));
			case t_int:
				return CProgramRuntime::AcquireNewIntValue(*reinterpret_cast<std::int64_t*>((char*)constant->m_pConstant.data()));
			case t_double:
				return CProgramRuntime::AcquireNewDoubleValue(*reinterpret_cast<double*>((char*)constant->m_pConstant.data()));;
		}
	}

	assert(false);
	return nullptr;
}
