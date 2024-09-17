#include "ast.hpp"

#include "linter/punctuation.hpp"
#include "linter/expressions/operator.hpp"
#include "linter/expressions/operand.hpp"

#include <cassert>
#include <sstream>

AbstractSyntaxTree AbstractSyntaxTree::CreateAST(VectorOf<CLinterOperand*>& operands, VectorOf<CLinterOperator*>& operators)
{
	assert(!operands.empty());
	AbstractSyntaxTree root{};

	root.CreateRecursively(operands, operators);

	return root;
}
void AbstractSyntaxTree::CreateLeaf(VectorOf<CLinterOperand*>& operands, VectorOf<CLinterOperator*>& operators)
{
	assert(operands.size() == 1u && operators.empty());
	type = NodeType::Operand;
	m_pOperand = operands.front();
	operands.clear();


}

void AbstractSyntaxTree::CreateRecursively(VectorOf<CLinterOperand*>& operands, VectorOf<CLinterOperator*>& operators)
{
	if (operands.empty()) {
		assert(operators.empty());
		return;
	}

	if (operands.size() == 1) {
		assert(operators.empty());
		return CreateLeaf(operands, operators);
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

	//I am an operator
	type = NodeType::Operator;
	m_pOperator = *itr1;

	//check size to avoid unnecessary allocations
	if (lhsOperands.size()) {
		left = std::make_shared<AbstractSyntaxTree>();
		left->CreateRecursively(lhsOperands, lhsOperators);
	}
	if (rhsOperands.size()) {
		right = std::make_shared<AbstractSyntaxTree>();
		right->CreateRecursively(rhsOperands, rhsOperators);
	}

}

OperatorIterator AbstractSyntaxTree::FindLowestPriorityOperator(VectorOf<CLinterOperator*>& operators)
{
	assert(!operators.empty());

	auto itr1 = operators.begin();
	OperatorPriority lowestPriorityOperator{ op_failure };
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

	const auto v = IsLeaf() ? m_pOperand->ToString() : m_pOperator->ToString();

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
