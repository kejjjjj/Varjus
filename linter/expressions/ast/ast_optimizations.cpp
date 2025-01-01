#ifdef OPTIMIZATIONS

#include "ast_main.hpp"
#include "linter/functions/stack.hpp"
#include "linter/functions/memory/manager.hpp"

#include "linter/optimizations/values/types/opt_value.hpp"
#include "linter/optimizations/values/opt_operators.hpp"

#include <iostream>
#include <cassert>

IConstEvalValue* VariableASTNode::GetConstEval(CMemory* const owner) noexcept {
	auto& vars = owner->m_VariableManager;

	if (auto variable = vars->GetVariableByIndex(m_uIndex)) {
		return variable->m_pConstEval;
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

	const auto& vars = owner->m_VariableManager;

	if (auto variable = vars->GetVariableByIndex(operand->GetVariable()->m_uIndex)) {
		return variable->m_bIsConstEval;
	}

	return false;
}
void AbstractSyntaxTree::OptimizeBranches(CMemory* const owner, ASTNode& node)
{
	if (!node || node->IsLeaf())
		return;

	// Recursively process children first
	if (node->left) {
		OptimizeBranches(owner, node->left);
	}
	if (node->right) {
		OptimizeBranches(owner, node->right);
	}

	// Call OptimizeNodes on the current branch after processing its children
	OptimizeNodes(owner, node);
}
void AbstractSyntaxTree::OptimizeNodes(CMemory* const owner, std::shared_ptr<AbstractSyntaxTree>& node)
{
	if (!node || node->IsLeaf())
		return;

	auto left = node->left.get();
	auto right = node->right.get();

	if (IsConstEval(owner, left) && IsConstEval(owner, right)) {
		auto lhs = left->GetConstEval(owner);
		auto rhs = right->GetConstEval(owner);

		auto result = OPT_ADDITION(lhs, rhs);

		lhs->Release();
		rhs->Release();
		
		if (!result) //something that cannot be determined at this point
			return;

		// Create the new constant node
		auto newValue = result->ToAST();

		// Transfer ownership of left and right children to the new node
		newValue->left = std::move(node->left->left);
		newValue->right = std::move(node->right->right);
		
		node = newValue;

		result->Release();
	}

	if (auto parent = node->parent.lock()) { 
		return OptimizeNodes(owner, parent);
	}
}


#endif
