#include "ast_main.hpp"


#include "linter/punctuation.hpp"
#include "linter/expressions/operator.hpp"
#include "linter/expressions/operand/operand.hpp"
#include "linter/expressions/operand/operand_includes.hpp"
#include "linter/expressions/postfix.hpp"
#include "linter/functions/stack.hpp"
#include "linter/functions/function.hpp"
#include "linter/modules/module.hpp"
#include "varjus_api/types/internal/references.hpp"
#include "linter/error.hpp"
#include "linter/expressions/operand/operand_fmt_string.hpp"

#include "linter/token.hpp"

#include <cassert>
#include <ranges>

AbstractSyntaxTree::~AbstractSyntaxTree() = default;



//TODO -> add a parameter called VSL("can discard meaningless expression")
ASTNode AbstractSyntaxTree::CreateAST(CMemory* const owner, Operands& operands, Operators& operators)
{
	assert(!operands.empty());

	auto root = GetLeaf(operands, operators);

	if (!root) {
		auto&& itr = FindLowestPriorityOperator(operators);
		root = std::make_shared<OperatorASTNode>((*itr)->GetToken()->m_oSourcePosition, (*itr)->GetPunctuation());
	}
	
	root->CreateRecursively(root, owner, operands, operators);
	return root;
}

ASTNode AbstractSyntaxTree::GetLeaf(Operands& operands, [[maybe_unused]] Operators& operators)
{

	ASTNode node;

	if (operands.size() == 1u) {
		assert(operators.empty());
		node = operands.front()->ToAST();
		operands.clear();

		assert(node != nullptr);
	}

	return node;
}


void AbstractSyntaxTree::CreateRecursively(ASTNode& _this, CMemory* const owner, Operands& operands, Operators& operators)
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

	auto lhsOperands = Operands(operands.begin(), operandLhs);
	auto rhsOperands = Operands(operandRhs, operands.end());

	auto lhsOperators = Operators(operators.begin(), opLhs);
	auto rhsOperators = Operators(opRhs, operators.end());

	if ((*itr1)->GetPriority() == op_conditional) {
		return CreateTernary(_this, owner, lhsOperands, lhsOperators, rhsOperands, rhsOperators);
	} else if ((*itr1)->GetPriority() == op_conditional2) {
		owner->GetModule()->PushError(VSL("\":\" can't be used in this context.. did you intend to use \"?:\""), (*itr1)->GetToken()->m_oSourcePosition);
		return;
	}

	assert(_this->IsOperator());

	//check size to avoid unnecessary allocations
	if (lhsOperands.size()) 
	{

		if (_this->left = GetLeaf(lhsOperands, lhsOperators), !_this->left) {
			const OperatorIterator l = FindLowestPriorityOperator(lhsOperators);

			_this->left = std::make_shared<OperatorASTNode>((*l)->GetToken()->m_oSourcePosition, (*l)->GetPunctuation());
			CreateRecursively(_this->left, owner, lhsOperands, lhsOperators);

		}
	} if (rhsOperands.size()) {
		if (_this->right = GetLeaf(rhsOperands, rhsOperators), !_this->right) {
			const OperatorIterator l = FindLowestPriorityOperator(rhsOperators);


			_this->right = std::make_shared<OperatorASTNode>((*l)->GetToken()->m_oSourcePosition, (*l)->GetPunctuation());
			CreateRecursively(_this->right, owner, rhsOperands, rhsOperators);
			
		}
	}

	if (_this->IsAssignment()) {
		_this->CheckConstness(owner);
		_this->CheckSelfCapture(owner);
	}
}
void AbstractSyntaxTree::CreateTernary(ASTNode& self, CMemory* const owner, Operands& lhs_operands,
 Operators& lhs_operators, Operands& rhs_operands, Operators& rhs_operators)
{
	//everything to the left of the ? is the condition
	ASTNode condition = AbstractSyntaxTree::CreateAST(owner, lhs_operands, lhs_operators);

	auto it = std::ranges::find(rhs_operators, op_conditional2, [](const CLinterOperator* o) { return o->GetPriority(); });

	if (it == rhs_operators.end()) {
		owner->GetModule()->PushError(VSL("expected a \":\" after \"?\" condition (misleading code position lol)"), condition->GetCodePosition());
		return;
	}

	const auto distance = std::distance(rhs_operators.begin(), it);

	const auto trueOperand = rhs_operands.begin() + distance + 1;
	const auto trueOperatorEnd = rhs_operators.begin() + distance;

	auto trueOperands = Operands(rhs_operands.begin(), trueOperand);
	auto trueOperators = Operators(rhs_operators.begin(), trueOperatorEnd);

	assert(trueOperands.size());
	ASTNode trueExpression = AbstractSyntaxTree::CreateAST(owner, trueOperands, trueOperators);

	auto falseOperands = Operands(trueOperand, rhs_operands.end());
	auto falseOperators = Operators(trueOperatorEnd + 1, rhs_operators.end());

	assert(falseOperands.size());
	ASTNode falseExpression = AbstractSyntaxTree::CreateAST(owner, falseOperands, falseOperators);

	self = std::make_shared<TernaryASTNode>(
		condition->GetCodePosition(),
		condition, 
		trueExpression, 
		falseExpression
	);

}
OperatorIterator AbstractSyntaxTree::FindLowestPriorityOperator(Operators& operators)
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
bool AbstractSyntaxTree::IsAssignment() const noexcept
{
	return GetOperator()->m_ePunctuation >= p_assign && GetOperator()->m_ePunctuation <= p_swap;
}
void AbstractSyntaxTree::CheckConstness(CMemory* const owner) const
{
	if (GetOperator()->m_ePunctuation == p_swap) {
		if (left->IsVariable() && left->GetVariable()->m_bIsConst) {
			owner->GetModule()->PushError(VSL("lhs is declared const"), left->m_oApproximatePosition);
			return;
		}
		if (right->IsVariable() && right->GetVariable()->m_bIsConst) {
			owner->GetModule()->PushError(VSL("rhs is declared const"), right->m_oApproximatePosition);
			return;
		}
	}

	if (left->IsVariable() && left->GetVariable()->m_bIsConst) {
		owner->GetModule()->PushError(VSL("lhs is declared const"), left->m_oApproximatePosition);
		return;
	}
}
void AbstractSyntaxTree::CheckSelfCapture(CMemory* const owner)
{
	if (owner->IsStack() && IsSelfReferencingCapture(left.get(), right.get())) {
		left->GetVariable()->m_bSelfCapturing = true;
	}
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


VariableASTNode::VariableASTNode(const CodePosition& pos, CLinterVariable* const var)
	: AbstractSyntaxTree(pos), CCrossModuleReference(*var),
	m_bGlobalVariable(var->IsGlobal()),
	m_bIsConst(var->m_bConst){}
VariableASTNode::~VariableASTNode() = default;


FunctionASTNode::FunctionASTNode(const CodePosition& pos, CLinterFunction* const func) 
	: AbstractSyntaxTree(pos),
	CCrossModuleReference(*func){}

ConstantASTNode::ConstantASTNode(const CodePosition& pos, const VarjusString& data, EValueType datatype)
	: AbstractSyntaxTree(pos), m_pConstant(data), m_eDataType(datatype) {
}
ConstantASTNode::~ConstantASTNode() = default;

#include "varjus_api/types/default.hpp"
ASTNode ConstantASTNode::FromIValue(const ConstantASTNode* _this, IValue* const value) noexcept
{
	assert(value && value->Type() <= t_string);

	return std::make_shared<ConstantASTNode>(
		_this->GetCodePosition(), 
		value->ValueAsBytes(), 
		value->Type()
	);
}

ArrayASTNode::ArrayASTNode(const CodePosition& pos, ExpressionList&& expressions)
	: AbstractSyntaxTree(pos), m_oExpressions(std::move(expressions)) {
}
ArrayASTNode::~ArrayASTNode() = default;

ObjectASTNode::ObjectASTNode(const CodePosition& pos, VectorOf<KeyValue<std::size_t, ASTNode>>&& expressions)
	: AbstractSyntaxTree(pos), m_oAttributes(std::move(expressions)) {
}
ObjectASTNode::~ObjectASTNode() = default;

TernaryASTNode::TernaryASTNode(const CodePosition& pos, ASTNode& value, ASTNode& m_true, ASTNode& m_false)
	: AbstractSyntaxTree(pos),
	m_pOperand(std::move(value)),
	m_pTrue(std::move(m_true)),
	m_pFalse(std::move(m_false)) {}
TernaryASTNode::~TernaryASTNode() = default;

LambdaASTNode::LambdaASTNode(const CodePosition& pos, RuntimeFunction&& operand, VectorOf<CCrossModuleReference>&& captures)
	: AbstractSyntaxTree(pos),
	m_pLambda(std::move(operand)), m_oVariableCaptures(std::move(captures)) {
}
LambdaASTNode::~LambdaASTNode() = default;


FmtStringASTNode::FmtStringASTNode(const CodePosition& pos, VectorOf<FmtStringAST>&& nodes)
	: AbstractSyntaxTree(pos), m_oNodes(std::move(nodes)) {}
FmtStringASTNode::~FmtStringASTNode() = default;