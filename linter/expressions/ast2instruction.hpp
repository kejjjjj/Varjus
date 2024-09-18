#pragma once

#include "definitions.hpp"
#include "globalEnums.hpp"

enum Instruction : std::uint8_t;

class AbstractSyntaxTree;
class COperation;
class COperand;

class CLinterOperand;

//converts an AST to a list of instructions
class AstToInstructionConverter
{
public:
	AstToInstructionConverter() = delete;
	AstToInstructionConverter(const AbstractSyntaxTree& target);
	~AstToInstructionConverter();

	Success Convert();

private:
	const AbstractSyntaxTree* ConvertRecursively(const AbstractSyntaxTree* tree);

	Instruction LoadInstruction(const CLinterOperator* op);
	std::unique_ptr<COperand> LoadFirstOperand(const CLinterOperand* operand);
	std::unique_ptr<COperand> LoadSecondOperand(const CLinterOperand* operand);

	const AbstractSyntaxTree& m_oAST;
	VectorOf<COperation> m_oOpCodes;

};
