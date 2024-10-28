#include "runtime/structure.hpp"
#include "runtime/functions/rtfunction.hpp"
#include "runtime/values/simple.hpp"
#include "runtime/values/simple_operators.hpp"
#include "runtime/values/integer.hpp"
#include "runtime/values/double.hpp"

#include "runtime/variables.hpp"
#include "runtime/runtime.hpp"

#include "linter/expressions/ast.hpp"

#include <cassert>

bool CRuntimeExpression::Execute([[maybe_unused]]CFunction* const thisFunction)
{

	[[maybe_unused]] const auto result = Evaluate(thisFunction, m_pAST.get());
	
	//TODO: make sure that all objects from the pool get released

	return true;
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

	/*
	TODO: Possibility to call AcquireNewValue with a value parameter for a quick initialization
	*/

	switch (node->As<const OperatorASTNode*>()->m_ePunctuation) {
	case p_assign:
		result = OP_ASSIGNMENT(lhs->GetOwner(), rhs);
		break;
	case p_add:
		result = OP_ADDITION(lhs, rhs);
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

		union {
			IValue* undefinedValue;
			CIntValue* intValue;
			CDoubleValue* doubleValue;
		}v{};

		switch (constant->m_eDataType) {
			case t_undefined:
				v.undefinedValue = CProgramRuntime::AcquireNewValue();
				return v.undefinedValue;

			case t_int:
				v.intValue = CProgramRuntime::AcquireNewIntValue();
				*v.intValue = CIntValue(*reinterpret_cast<std::int64_t*>((char*)constant->m_pConstant.data()));
				return v.intValue;

			case t_double:
				v.doubleValue = CProgramRuntime::AcquireNewDoubleValue();
				*v.doubleValue = CDoubleValue(*reinterpret_cast<double*>((char*)constant->m_pConstant.data()));
				return v.doubleValue;
		}
	}

	assert(false);
	return nullptr;
}
