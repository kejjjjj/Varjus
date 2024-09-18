#include "ast2instruction.hpp"
#include "ast.hpp"
#include "instructions/instructions.hpp"
#include "instructions/register.hpp"
#include "instructions/operand.hpp"

#include "linter/declarations/stack.hpp"

#include "linter/punctuation.hpp"
#include "operand.hpp"
#include "operator.hpp"

#include <cassert>

AstToInstructionConverter::AstToInstructionConverter(const AbstractSyntaxTree& target) : m_oAST(target)
{

}
AstToInstructionConverter::~AstToInstructionConverter() = default;


Success AstToInstructionConverter::Convert()
{



	return failure;
};

const AbstractSyntaxTree* AstToInstructionConverter::ConvertRecursively(const AbstractSyntaxTree* tree)
{
	if (!tree)
		return nullptr;

	if (tree->IsLeaf()) {
		assert(tree->m_pOperand);
		return tree;
	}

	const auto leftBranch = ConvertRecursively(tree->left.get());
	const auto rightBranch= ConvertRecursively(tree->right.get());
	const auto op = tree;

	assert(op && op->m_pOperator);
	assert(leftBranch && rightBranch);

	const auto leftOperand = leftBranch->m_pOperand;
	[[maybe_unused]]const auto rightOperand = rightBranch->m_pOperand;

	COperation operation(LoadInstruction(op->m_pOperator));
	operation.AddOperand(LoadFirstOperand(leftOperand));
	operation.AddOperand(LoadSecondOperand(leftOperand));


	return nullptr;

}
Instruction AstToInstructionConverter::LoadInstruction(const CLinterOperator* op)
{
#pragma warning(push)
#pragma warning(disable : 4061)
	switch (op->GetPriority()) {
	case p_add:
		return ADD;
	case p_assign:
		return MOV;
	default:
		return NOP;
	}
#pragma warning(pop)

}
std::unique_ptr<COperand> AstToInstructionConverter::LoadFirstOperand(const CLinterOperand* operand)
{

	if (operand->IsVariable()) {
		return std::make_unique<CRegisterOperand>(EDX);

	}

	return std::make_unique<CRegisterOperand>(EAX);
}
std::unique_ptr<COperand> AstToInstructionConverter::LoadSecondOperand(const CLinterOperand* operand)
{

	if (operand->IsVariable()) {
		auto v = operand->GetVariable();
		return std::make_unique<CVariableOperand>(v->m_uIndex, v);

	}

	return std::make_unique<CIntOperand>(operand->GetOperand()->ToInt());
}

