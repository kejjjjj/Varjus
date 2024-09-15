#include "ast2instruction.hpp"
#include "ast.hpp"
#include "instructions/instructions.hpp"

#include <cassert>

AstToInstructionConverter::AstToInstructionConverter(const AbstractSyntaxTree& target) : m_oAST(target)
{

}
AstToInstructionConverter::~AstToInstructionConverter() = default;


Success AstToInstructionConverter::Convert()
{



	return failure;
};

Success AstToInstructionConverter::ConvertRecursively(const AbstractSyntaxTree* tree)
{
	if (!tree)
		return failure;


	const auto leftOperand = ConvertRecursively(tree->left.get());
	const auto rightOperand = ConvertRecursively(tree->right.get());
	const auto op = tree;

	assert(op && op->m_pOperator);
	assert(leftOperand && rightOperand);

	return failure;

}
