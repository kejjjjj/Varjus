#include "ast.hpp"

#include "linter/punctuation.hpp"
#include "linter/expressions/operator.hpp"
#include "linter/expressions/operand.hpp"

#include "linter/token.hpp"

#include <cassert>
#include <sstream>
#include <iostream>

std::unique_ptr<AbstractSyntaxTree> AbstractSyntaxTree::CreateAST(VectorOf<CLinterOperand*>& operands, VectorOf<CLinterOperator*>& operators)
{
	assert(!operands.empty());

	auto root = GetPolymorphic(operands, operators);

	if (!root)
		root = std::make_unique<OperatorASTNode>((*FindLowestPriorityOperator(operators))->GetPunctuation());

	root->CreateRecursively(operands, operators);
	return root;
}

std::unique_ptr<AbstractSyntaxTree> AbstractSyntaxTree::GetPolymorphic(VectorOf<CLinterOperand*>& operands, VectorOf<CLinterOperator*>& operators)
{

	std::unique_ptr<AbstractSyntaxTree> node;

	if (operands.size() == 1) {
		assert(operators.empty());
		const auto operand = operands.front();
		if (operand->IsImmediate()) {
			node = std::make_unique<ConstantASTNode>(std::move(operand->GetOperandRaw()));
		} else if (operand->IsVariable()) {
			node = std::make_unique<VariableASTNode>(std::move(operand->GetOperandRaw()));
		} else if (operand->IsExpression()) {
			node = operand->ExpressionToAST();
		}

		operands.clear();
	}
	
	return node;
}

void AbstractSyntaxTree::CreateRecursively(VectorOf<CLinterOperand*>& operands, VectorOf<CLinterOperator*>& operators)
{
	if (operands.empty()) {
		assert(operators.empty());
		return;
	}

	assert(!operands.empty() && !operators.empty());

	const OperatorIterator itr1 = FindLowestPriorityOperator(operators);
	
	const OperatorIterator opLhs = itr1;
	const OperatorIterator opRhs = std::next(opLhs);

	const auto operandLhs = operands.begin() + std::distance(operators.begin(), itr1) + 1;
	const auto operandRhs = operands.begin() + std::distance(operators.begin(), itr1) + 1;


	auto lhsOperands = VectorOf<CLinterOperand*>(operands.begin(), operandLhs);
	auto rhsOperands = VectorOf<CLinterOperand*>(operandRhs, operands.end());

	auto lhsOperators = VectorOf<CLinterOperator*>(operators.begin(), opLhs);
	auto rhsOperators = VectorOf<CLinterOperator*>(opRhs, operators.end());

	assert(IsOperator());

	//check size to avoid unnecessary allocations
	if (lhsOperands.size()) {
		if (left = GetPolymorphic(lhsOperands, lhsOperators), !left) {
			const OperatorIterator l = FindLowestPriorityOperator(lhsOperators);
			left = std::make_shared<OperatorASTNode>((*l)->GetPunctuation());
			left->CreateRecursively(lhsOperands, lhsOperators);
		}
	}
	if (rhsOperands.size()) {
		if (right = GetPolymorphic(rhsOperands, rhsOperators), !right) {

			const OperatorIterator l = FindLowestPriorityOperator(rhsOperators);
			right = std::make_shared<OperatorASTNode>((*l)->GetPunctuation());
			right->CreateRecursively(rhsOperands, rhsOperators);
		}
	}

}

OperatorIterator AbstractSyntaxTree::FindLowestPriorityOperator(VectorOf<CLinterOperator*>& operators)
{
	assert(!operators.empty());

	auto itr1 = operators.begin();
	OperatorPriority lowestPriorityOperator{ op_postfix };
	OperatorIterator lowestPriorityOperatorItr{ itr1 };

	for (; itr1 != operators.end(); ++itr1) {

		if ((*itr1)->GetPriority() <= lowestPriorityOperator) {
			lowestPriorityOperator = (*itr1)->GetPriority();
			lowestPriorityOperatorItr = itr1;
		}
	};

	return lowestPriorityOperatorItr;
}


std::string AbstractSyntaxTree::ToString() const noexcept
{
	if (!this)
		return "";

	std::stringstream result;

	auto levels = VectorOf<VectorOf<std::string>>{};
	ToStringInternal(0u, 0, GetLeftBranchDepth(), levels);

	for (const auto& values : levels) {
		for (const auto& value : values) {
			result << value << " ";
		}
		result << "\n";
	}

	return result.str();
}

void AbstractSyntaxTree::ToStringInternal(std::size_t depth, std::ptrdiff_t horzAlign, std::size_t totalWidth, VectorOf<VectorOf<std::string>>& levels) const noexcept
{
	if (!this)
		return;

	const auto v = ToStringPolymorphic();

	if (levels.size() <= depth)
		levels.resize(depth+1u);

	levels[depth].push_back(std::string(totalWidth - horzAlign, ' ') + v);

	left->ToStringInternal(depth + 1, horzAlign - 1, totalWidth, levels);
	right->ToStringInternal(depth + 1, horzAlign + 1, totalWidth, levels);
}

std::size_t AbstractSyntaxTree::GetLeftBranchDepth() const noexcept
{
	if (!left && !right)
		return 0u;

	std::size_t depth{ 1u };

	auto ptr = this;

	while (ptr) {

		std::size_t lDepth = ptr->left ? ptr->left->GetLeftBranchDepth() : 0u;
		std::size_t rDepth = ptr->right ? ptr->right->GetLeftBranchDepth() : 0u;

		if (!lDepth && !rDepth)
			break;


		depth++;
		ptr = lDepth > rDepth ? &*ptr->left : &*ptr->right;
	}

	return depth;
}


/***********************************************************************
 > 
***********************************************************************/
VariableASTNode::~VariableASTNode() = default;

std::string VariableASTNode::ToStringPolymorphic() const noexcept
{
	assert(m_pOperand);
	assert(m_pOperand->Type() == identifier);

	return m_pOperand->GetIdentifier()->GetResult()->Source();

}

ConstantASTNode::~ConstantASTNode() = default;
std::string ConstantASTNode::ToStringPolymorphic() const noexcept
{
	assert(m_pOperand);
	assert(m_pOperand->Type() == identifier);
	return m_pOperand->GetIdentifier()->GetResult()->Source();

}

std::string OperatorASTNode::ToStringPolymorphic() const noexcept
{
	return std::string(punctuations[m_ePunctuation].m_sIdentifier);
}
