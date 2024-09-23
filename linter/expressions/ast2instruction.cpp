#include "ast2instruction.hpp"
#include "ast.hpp"
#include "instructions/instructions.hpp"
#include "instructions/register.hpp"
#include "instructions/operand.hpp"

#include "linter/declarations/stack.hpp"

#include "linter/punctuation.hpp"
#include "operand.hpp"
#include "operator.hpp"
#include "ast_registers.hpp"

#include <cassert>
#include <iostream>


AstToInstructionConverter::AstToInstructionConverter(const AbstractSyntaxTree& target, const CRegisters& registers) : m_oAST(target), m_oRefRegisters(registers)
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
	//const auto rightOperand = rightBranch->m_pOperand;
	
	if (!m_oRefRegisters.OperandIsLoaded(leftOperand)) {
		[[maybe_unused]] const auto operation = LoadOperandToRegister(leftOperand);
	}



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
std::unique_ptr<COperation> AstToInstructionConverter::LoadOperandToRegister([[maybe_unused]]const CLinterOperand* operand)
{
	auto operation = std::make_unique<COperation>(MOV);

	if (operand->IsImmediate()) {
		const auto reg = m_oRefRegisters.Get(m_oRefRegisters.AllocateRegister(EAX));
		reg->m_pValue = operand;
		operation->AddOperand(std::make_unique<CRegisterOperand>(reg->m_pRegister->m_eRegister));

		//hardcode to int for now
		operation->AddOperand(std::make_unique<CIntOperand>(operand->GetOperand()->ToInt()));
	}

	else if (operand->IsVariable()) {
		const auto reg = m_oRefRegisters.Get(m_oRefRegisters.AllocateRegister(EDX));
		reg->m_pValue = operand;

		operation->AddOperand(std::make_unique<CRegisterOperand>(reg->m_pRegister->m_eRegister));
		operation->AddOperand(std::make_unique<CVariableOperand>(operand->GetVariable()->m_uIndex, operand->GetVariable()));
	}

	return operation;
}
