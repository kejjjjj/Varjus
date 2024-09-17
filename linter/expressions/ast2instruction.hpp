#pragma once

#include "definitions.hpp"
#include "globalEnums.hpp"

class AbstractSyntaxTree;
class COperation;

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


	const AbstractSyntaxTree& m_oAST;
	VectorOf<COperation> m_oOpCodes;

};
