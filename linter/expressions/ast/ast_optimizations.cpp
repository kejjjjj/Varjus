#ifdef OPTIMIZATIONS

#include "ast_main.hpp"
#include "linter/functions/stack.hpp"
#include "linter/functions/memory/manager.hpp"

#include "linter/optimizations/values/types/opt_value.hpp"
#include "linter/optimizations/values/opt_operators.hpp"
#include "linter/optimizations/variables/opt_variables.hpp"


#include <cassert>

IConstEvalValue* VariableASTNode::GetConstEval(CMemory* const owner) noexcept {
	auto& vars = owner->m_ConstEvalVariableManager;
	assert(vars);
	if (auto variable = vars->GetVariableByIndex(m_uIndex)) {
		assert(variable->m_pConstEval);
		return variable->m_pConstEval->GetValue();
	}

	assert(false);
	return nullptr;
}

bool AbstractSyntaxTree::IsConstEval(CMemory* const owner, const AbstractSyntaxTree* operand)
{
	if (operand->IsConstant())
		return true;

	if (!operand->IsVariable())
		return false;

	const auto& vars = owner->m_ConstEvalVariableManager;
	assert(vars);
	if (auto variable = vars->GetVariableByIndex(operand->GetVariable()->m_uIndex)) {
		return !!variable->m_pConstEval;
	}

	return false;
}
void AbstractSyntaxTree::OptimizeBranches(CMemory* const owner, ASTNode& node)
{
	assert(node);

	//pretty much just CRuntimeExpression::Evaluate

	if (node->IsLeaf())
		return;

	OptimizeBranches(owner, node->left);
	OptimizeBranches(owner, node->right);

	OptimizeNodes(owner, node);
}

// AKA fold constants
void AbstractSyntaxTree::OptimizeNodes(CMemory* const owner, std::shared_ptr<AbstractSyntaxTree>& node)
{
	if (!node || node->IsLeaf())
		return;

	auto left = node->left.get();
	auto right = node->right.get();

	if (IsConstEval(owner, left) && IsConstEval(owner, right)) {
		auto lhs = left->GetConstEval(owner);
		auto rhs = right->GetConstEval(owner);

		auto& op = m_oOptOperatorTable[node->GetOperator()->m_ePunctuation];

		if (!op) {
			if(!lhs->HasOwner()) lhs->Release();
			if(!rhs->HasOwner()) rhs->Release();
			return;
		}

		auto result = op(lhs, rhs);

		if (!lhs->HasOwner()) lhs->Release();
		if (!rhs->HasOwner()) rhs->Release();
		
		if (!result) //something that cannot be determined at this point
			return;

		// Create the new constant node
		auto newValue = result->ToAST();

		assert(!left->left);
		assert(!right->right);
		
		node = newValue;

		if(!result->HasOwner())
			result->Release();
	}
}
void AbstractSyntaxTree::OptimizeLeaf(CMemory* const owner, ASTNode& node)
{
	assert(node && node->IsLeaf());

	if (!node->IsVariable() || !node->IsConstEval(owner, node.get()))
		return;

	auto constEval = node->GetConstEval(owner);
	assert(constEval && constEval->HasOwner());

	node = constEval->ToAST();
}

#endif
