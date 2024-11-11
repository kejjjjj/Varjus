#include "ast.hpp"


#include "linter/punctuation.hpp"
#include "linter/expressions/operator.hpp"
#include "linter/expressions/operand.hpp"
#include "linter/expressions/postfix.hpp"
#include "linter/functions/stack.hpp"

#include "linter/token.hpp"

#include <cassert>
#include <sstream>

AbstractSyntaxTree::AbstractSyntaxTree() = default;
AbstractSyntaxTree::~AbstractSyntaxTree() = default;

std::unique_ptr<AbstractSyntaxTree> AbstractSyntaxTree::CreateAST(VectorOf<CLinterOperand*>& operands, VectorOf<CLinterOperator*>& operators)
{
	assert(!operands.empty());

	auto root = GetLeaf(operands, operators);

	if (!root)
		root = std::make_unique<OperatorASTNode>((*FindLowestPriorityOperator(operators))->GetPunctuation());

	root->CreateRecursively(operands, operators);
	return root;
}

std::unique_ptr<AbstractSyntaxTree> AbstractSyntaxTree::GetLeaf(VectorOf<CLinterOperand*>& operands, [[maybe_unused]] VectorOf<CLinterOperator*>& operators)
{

	std::unique_ptr<AbstractSyntaxTree> node;

	if (operands.size() == 1u) {
		assert(operators.empty());
		node = operands.front()->ToAST();
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
		if (left = GetLeaf(lhsOperands, lhsOperators), !left) {
			const OperatorIterator l = FindLowestPriorityOperator(lhsOperators);
			left = std::make_shared<OperatorASTNode>((*l)->GetPunctuation());
			left->CreateRecursively(lhsOperands, lhsOperators);
		}
	}
	if (rhsOperands.size()) {
		if (right = GetLeaf(rhsOperands, rhsOperators), !right) {

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

ConstantASTNode::ConstantASTNode(const std::string& data, EValueType datatype)
	: m_pConstant(data), m_eDataType(datatype) {}
ConstantASTNode::~ConstantASTNode() = default;

ArrayASTNode::ArrayASTNode(ExpressionList&& expressions) : m_oExpressions(std::move(expressions)) {}
ArrayASTNode::~ArrayASTNode() = default;
