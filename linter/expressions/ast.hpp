#pragma once

#include <memory>

#include "definitions.hpp"

class AbstractSyntaxTree;
using ASTNode = std::shared_ptr<AbstractSyntaxTree>;

class CLinterOperand;
class CLinterOperator;

class AbstractSyntaxTree final
{
	friend class AstToInstructionConverter;
	enum NodeType { Operator, Operand };
public:

	AbstractSyntaxTree() = default;
	AbstractSyntaxTree(NodeType n) : type(n) {}
	~AbstractSyntaxTree() = default;
	[[nodiscard]] constexpr bool IsLeaf() const noexcept { return type == Operand; }

	ASTNode left;
	ASTNode right;
	NodeType type = Operator;

	CLinterOperand* m_pOperand = nullptr;
	CLinterOperator* m_pOperator = nullptr;

public:
	[[nodiscard]] static AbstractSyntaxTree CreateAST(VectorOf<CLinterOperand*>& operands, VectorOf<CLinterOperator*>& operators);
	[[nodiscard]] std::string ToString() const noexcept;

private:
	[[nodiscard]] static OperatorIterator FindLowestPriorityOperator(VectorOf<CLinterOperator*>& operators);

	void CreateLeaf(VectorOf<CLinterOperand*>& operands, VectorOf<CLinterOperator*>& operators);
	void CreateRecursively(VectorOf<CLinterOperand*>& operands, VectorOf<CLinterOperator*>& operators);

	void ToStringInternal(std::size_t depth, std::ptrdiff_t horzAlign, std::size_t totalWidth, VectorOf<VectorOf<std::string>>& levels) const noexcept;

	[[nodiscard]] std::size_t GetLeftBranchDepth() const noexcept;

};

