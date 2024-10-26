#pragma once

#include <memory>
#include <iostream>
#include "definitions.hpp"

#include "linter/punctuation.hpp"

class AbstractSyntaxTree;
using ASTNode = std::shared_ptr<AbstractSyntaxTree>;

class CLinterOperand;
class CLinterOperator;
struct COperandBase;

class AbstractSyntaxTree
{
	friend class AstToInstructionConverter;
public:


	AbstractSyntaxTree();
	virtual ~AbstractSyntaxTree();

	[[nodiscard]] virtual constexpr bool IsLeaf() const noexcept	 { return false; }
	[[nodiscard]] virtual constexpr bool IsOperator() const noexcept { return false; }
	[[nodiscard]] virtual constexpr bool IsVariable() const noexcept { return false; }
	[[nodiscard]] virtual constexpr bool IsConstant() const noexcept { return false; }

	template<typename T>
	[[nodiscard]] inline constexpr T As() const noexcept {
		static_assert(std::is_pointer_v<T> || std::is_reference_v<T>, "T must either be a pointer or a reference");
		return dynamic_cast<T>(this); 
	}

	template<typename T>
	[[nodiscard]] inline constexpr T As() noexcept {
		static_assert(std::is_pointer_v<T> || std::is_reference_v<T>, "T must either be a pointer or a reference");
		return dynamic_cast<T>(this);
	}

	ASTNode left;
	ASTNode right;

public:
	[[nodiscard]] static std::unique_ptr<AbstractSyntaxTree> CreateAST(VectorOf<CLinterOperand*>& operands, VectorOf<CLinterOperator*>& operators);
	[[nodiscard]] std::string ToString() const noexcept;

private:
	[[nodiscard]] virtual std::string ToStringPolymorphic() const noexcept { return ""; };

	static std::unique_ptr<AbstractSyntaxTree> GetPolymorphic(VectorOf<CLinterOperand*>& operands, VectorOf<CLinterOperator*>& operators);


protected:
	[[nodiscard]] static OperatorIterator FindLowestPriorityOperator(VectorOf<CLinterOperator*>& operators);

	void CreateRecursively(VectorOf<CLinterOperand*>& operands, VectorOf<CLinterOperator*>& operators);

	void ToStringInternal(std::size_t depth, std::ptrdiff_t horzAlign, std::size_t totalWidth, VectorOf<VectorOf<std::string>>& levels) const noexcept;

	[[nodiscard]] std::size_t GetLeftBranchDepth() const noexcept;


};

class VariableASTNode final : public AbstractSyntaxTree
{
	friend class AstToInstructionConverter;
	NONCOPYABLE(VariableASTNode);
public:
	VariableASTNode(std::unique_ptr<COperandBase>&& owner);
	~VariableASTNode();

	[[nodiscard]] constexpr bool IsVariable() const noexcept override { return true; }
	[[nodiscard]] std::string ToStringPolymorphic() const noexcept override;



private:
	
	std::unique_ptr<COperandBase> m_pOperand;
};

class ConstantASTNode final : public AbstractSyntaxTree
{
	NONCOPYABLE(ConstantASTNode);

public:

	ConstantASTNode(std::unique_ptr<COperandBase>&& owner);
	~ConstantASTNode();

	[[nodiscard]] constexpr bool IsLeaf() const noexcept override { return true; }
	[[nodiscard]] constexpr bool IsConstant() const noexcept override { return true; }

	[[nodiscard]] std::string ToStringPolymorphic() const noexcept override;

private:
	std::unique_ptr<COperandBase> m_pOperand;
};

class OperatorASTNode final : public AbstractSyntaxTree
{
	friend class AstToInstructionConverter;

public:
	OperatorASTNode() = default;
	OperatorASTNode(Punctuation punc) : m_ePunctuation(punc) {}
	[[nodiscard]] constexpr bool IsOperator() const noexcept override { return true; }

	[[nodiscard]] std::string ToStringPolymorphic() const noexcept override;

//private:
	Punctuation m_ePunctuation;
};
