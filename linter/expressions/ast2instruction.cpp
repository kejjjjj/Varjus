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


//AstToInstructionConverter::AstToInstructionConverter(const AbstractSyntaxTree& target, const CRegisters& registers) : m_oAST(target), m_oRefRegisters(registers)
//{
//
//}
//AstToInstructionConverter::~AstToInstructionConverter() = default;
//
//
//Success AstToInstructionConverter::Convert()
//{
//
//
//
//	return failure;
//};
//
//Register AstToInstructionConverter::ConvertRecursively(const AbstractSyntaxTree* tree)
//{
//	if (!tree) {
//		assert(false);
//		return ANY_REGISTER;
//	}
//
//	Register reg = ANY_REGISTER;
//
//	if (tree->IsLeaf()) {
//
//		if (tree->IsVariable()) {
//			const auto var = tree->As<const VariableASTNode*>();
//			assert(var && var->m_pOperand);
//			const auto operand = var->m_pOperand;
//
//			if (!m_oRefRegisters.OperandIsLoaded(operand)) {
//				reg = m_oRefRegisters.AllocateRegister(EDX); //target edx when possible
//			} else {
//				reg = m_oRefRegisters.GetOperandRegister(operand);
//			}
//
//		} else if (tree->IsConstant()) {
//			reg = m_oRefRegisters.AllocateRegister(EAX); //target eax when possible
//		}
//	} else {
//		const auto lhs = ConvertRecursively(tree->left.get());
//		const auto rhs = ConvertRecursively(tree->right.get());
//		const auto op = tree;
//
//		assert(op && op->IsOperator());
//		assert(lhs != ANY_REGISTER && rhs != ANY_REGISTER);
//
//		const auto oper = tree->As<const OperatorASTNode*>();
//		assert(oper && oper->m_pOperator);
//
//		[[maybe_unused]]const auto insn = LoadInstruction(oper->m_pOperator);
//
//	}
//	return ANY_REGISTER;
//}
//Instruction AstToInstructionConverter::LoadInstruction(const CLinterOperator* op)
//{
//#pragma warning(push)
//#pragma warning(disable : 4061)
//	switch (op->GetPriority()) {
//	case p_add:
//		return ADD;
//	case p_assign:
//		return MOV;
//	default:
//		return NOP;
//	}
//#pragma warning(pop)
//
//}
//std::unique_ptr<COperation> AstToInstructionConverter::LoadOperandToRegister([[maybe_unused]]const CLinterOperand* operand)
//{
//	auto operation = std::make_unique<COperation>(MOV);
//
//	if (operand->IsImmediate()) {
//		const auto reg = m_oRefRegisters.Get(m_oRefRegisters.AllocateRegister(EAX));
//		reg->m_pValue = operand;
//		operation->AddOperand(std::make_unique<CRegisterOperand>(reg->m_pRegister->m_eRegister));
//
//		//hardcode to int for now
//		//operation->AddOperand(std::make_unique<CIntOperand>(operand->GetOperand()->ToInt()));
//	}
//
//	else if (operand->IsVariable()) {
//		const auto reg = m_oRefRegisters.Get(m_oRefRegisters.AllocateRegister(EDX));
//		reg->m_pValue = operand;
//
//		operation->AddOperand(std::make_unique<CRegisterOperand>(reg->m_pRegister->m_eRegister));
//		operation->AddOperand(std::make_unique<CVariableOperand>(operand->GetVariable()->m_uIndex, operand->GetVariable()));
//	}
//
//	return operation;
//}
