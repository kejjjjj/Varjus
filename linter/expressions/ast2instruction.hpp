#pragma once

#include "definitions.hpp"
#include "globalEnums.hpp"

#include "instructions/register.hpp"

enum Instruction : std::uint8_t;

class AbstractSyntaxTree;
class COperation;
class COperand;

class CLinterOperand;

class CRegisters;

//converts an AST to a list of instructions
class AstToInstructionConverter final
{
public:
	AstToInstructionConverter() = delete;
	AstToInstructionConverter(const AbstractSyntaxTree& target, const CRegisters& registers);
	~AstToInstructionConverter();

	Success Convert();

private:
	Register ConvertRecursively(const AbstractSyntaxTree* tree);

	Instruction LoadInstruction(const CLinterOperator* op);
	std::unique_ptr<COperation> LoadOperandToRegister(const CLinterOperand* operand);

	const AbstractSyntaxTree& m_oAST;
	VectorOf<std::unique_ptr<COperation>> m_oOpCodes;

	const CRegisters& m_oRefRegisters;
};
