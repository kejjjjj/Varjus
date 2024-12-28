#include "ast_main.hpp"


#include "linter/punctuation.hpp"
#include "linter/expressions/operator.hpp"
#include "linter/expressions/operand/operand.hpp"
#include "linter/expressions/operand/operand_includes.hpp"
#include "linter/expressions/postfix.hpp"
#include "linter/functions/stack.hpp"
#include "linter/functions/function.hpp"
#include "linter/modules/references.hpp"
#include "linter/error.hpp"

#include "linter/token.hpp"

#include <cassert>

AbstractSyntaxTree::~AbstractSyntaxTree() = default;

std::unique_ptr<AbstractSyntaxTree> AbstractSyntaxTree::CreateAST(CMemory* const owner,
	VectorOf<CLinterOperand*>& operands, VectorOf<CLinterOperator*>& operators)
{
	assert(!operands.empty());

	auto root = GetLeaf(operands, operators);

	if (!root) {
		auto&& itr = FindLowestPriorityOperator(operators);
		root = std::make_unique<OperatorASTNode>((*itr)->GetToken()->m_oSourcePosition, (*itr)->GetPunctuation());
	}
	root->CreateRecursively(owner, operands, operators);
	return root;
}

std::unique_ptr<AbstractSyntaxTree> AbstractSyntaxTree::GetLeaf(VectorOf<CLinterOperand*>& operands, [[maybe_unused]] VectorOf<CLinterOperator*>& operators)
{

	std::unique_ptr<AbstractSyntaxTree> node;

	if (operands.size() == 1u) {
		assert(operators.empty());
		node = operands.front()->ToAST();
		operands.clear();

		assert(node != nullptr);
	}

	return node;
}

void AbstractSyntaxTree::CreateRecursively(CMemory* const owner, 
	VectorOf<CLinterOperand*>& operands, VectorOf<CLinterOperator*>& operators)
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
			left = std::make_shared<OperatorASTNode>((*l)->GetToken()->m_oSourcePosition, (*l)->GetPunctuation());
			left->CreateRecursively(owner, lhsOperands, lhsOperators);


		}
	}
	if (rhsOperands.size()) {
		if (right = GetLeaf(rhsOperands, rhsOperators), !right) {
			const OperatorIterator l = FindLowestPriorityOperator(rhsOperators);
			right = std::make_shared<OperatorASTNode>((*l)->GetToken()->m_oSourcePosition, (*l)->GetPunctuation());
			right->CreateRecursively(owner, rhsOperands, rhsOperators);

		}
	}

	if (GetOperator()->m_ePunctuation >= p_assign && GetOperator()->m_ePunctuation <= p_assignment_bitwise_and) {
		if (left->IsVariable() && left->GetVariable()->m_bIsConst) {
			CLinterErrors::PushError("lhs is declared const", left->m_oApproximatePosition);
			return;
		}
		if (owner->IsStack() && IsSelfReferencingCapture(left.get(), right.get())) {
			left->GetVariable()->m_bSelfCapturing = true;
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

bool AbstractSyntaxTree::IsSelfReferencingCapture(const AbstractSyntaxTree* lhs, const AbstractSyntaxTree* rhs)
{
	//logic: lhs = rhs and rhs is a lambda and rhs captures lhs
	if (!lhs || !rhs)
		return false;

	if (!lhs->IsVariable())
		return false;

	if (!rhs->IsLambda())
		return false;

	auto variable = lhs->GetVariable();
	auto lambda = rhs->GetLambda();
	bool containsLHS = false;

	for (auto& c : lambda->m_oVariableCaptures) {
		if (*variable == c) {
			containsLHS = true;
			break;
		}
	}

	return containsLHS;
}

/***********************************************************************
 >
***********************************************************************/

VariableASTNode::VariableASTNode(const CodePosition& pos, CLinterVariable* const var)
	: AbstractSyntaxTree(pos), CCrossModuleReference(*var),
	m_bGlobalVariable(var->IsGlobal()),
	m_bIsConst(var->m_bConst){}

FunctionASTNode::FunctionASTNode(const CodePosition& pos, CLinterFunction* const func) 
	: AbstractSyntaxTree(pos),
	CCrossModuleReference(*func){}

ConstantASTNode::ConstantASTNode(const CodePosition& pos, const std::string& data, EValueType datatype)
	: AbstractSyntaxTree(pos), m_pConstant(data), m_eDataType(datatype) {
}
ConstantASTNode::~ConstantASTNode() = default;

ArrayASTNode::ArrayASTNode(const CodePosition& pos, ExpressionList&& expressions)
	: AbstractSyntaxTree(pos), m_oExpressions(std::move(expressions)) {
}
ArrayASTNode::~ArrayASTNode() = default;

ObjectASTNode::ObjectASTNode(const CodePosition& pos, VectorOf<KeyValue<std::size_t, UniqueAST>>&& expressions)
	: AbstractSyntaxTree(pos), m_oAttributes(std::move(expressions)) {
}
ObjectASTNode::~ObjectASTNode() = default;

TernaryASTNode::TernaryASTNode(const CodePosition& pos, CTernaryOperand* operand)
	: AbstractSyntaxTree(pos),
	m_pOperand(std::move(operand->m_pValue)),
	m_pTrue(std::move(operand->m_pTrue)),
	m_pFalse(std::move(operand->m_pFalse)) {
}
TernaryASTNode::~TernaryASTNode() = default;

LambdaASTNode::LambdaASTNode(const CodePosition& pos, RuntimeFunction&& operand, VectorOf<CCrossModuleReference>&& captures)
	: AbstractSyntaxTree(pos),
	m_pLambda(std::move(operand)), m_oVariableCaptures(std::move(captures)) {
}
LambdaASTNode::~LambdaASTNode() = default;