#pragma once

#include <memory>

#include "definitions.hpp"

class AbstractSyntaxTree;
using ASTNode = std::shared_ptr<AbstractSyntaxTree>;

class CLinterOperand;
class CLinterOperator;

class AbstractSyntaxTree
{
	friend class AstToInstructionConverter;
public:


	AbstractSyntaxTree() = default;
	virtual ~AbstractSyntaxTree() = default;
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
	[[nodiscard]] static std::shared_ptr<AbstractSyntaxTree> CreateAST(VectorOf<CLinterOperand*>& operands, VectorOf<CLinterOperator*>& operators);
	[[nodiscard]] std::string ToString() const noexcept;

private:
	[[nodiscard]] virtual std::string ToStringPolymorphic() const noexcept { return ""; };

	static std::shared_ptr<AbstractSyntaxTree> GetPolymorphic(VectorOf<CLinterOperand*>& operands, VectorOf<CLinterOperator*>& operators);


protected:
	[[nodiscard]] static OperatorIterator FindLowestPriorityOperator(VectorOf<CLinterOperator*>& operators);

	static void CreateRecursively(ASTNode& node, VectorOf<CLinterOperand*>& operands, VectorOf<CLinterOperator*>& operators);

	void ToStringInternal(std::size_t depth, std::ptrdiff_t horzAlign, std::size_t totalWidth, VectorOf<VectorOf<std::string>>& levels) const noexcept;

	[[nodiscard]] std::size_t GetLeftBranchDepth() const noexcept;


};

class VariableASTNode final : public AbstractSyntaxTree
{
	friend class AstToInstructionConverter;
public:
	VariableASTNode(CLinterOperand* owner) : m_pOperand(owner){}

	[[nodiscard]] constexpr bool IsVariable() const noexcept override { return true; }

	[[nodiscard]] std::string ToStringPolymorphic() const noexcept override;


private:

	CLinterOperand* m_pOperand = nullptr;
};

class ConstantASTNode final : public AbstractSyntaxTree
{
public:
	ConstantASTNode(CLinterOperand* owner) : m_pOperand(owner) {}

	[[nodiscard]] constexpr bool IsLeaf() const noexcept override { return true; }
	[[nodiscard]] constexpr bool IsConstant() const noexcept override { return true; }

	[[nodiscard]] std::string ToStringPolymorphic() const noexcept override;

private:

	CLinterOperand* m_pOperand = nullptr;
};

class OperatorASTNode final : public AbstractSyntaxTree
{
	friend class AstToInstructionConverter;

public:
	OperatorASTNode() = default;
	OperatorASTNode(CLinterOperator* owner) : m_pOperator(owner) {}
	[[nodiscard]] constexpr bool IsOperator() const noexcept override { return true; }

	[[nodiscard]] std::string ToStringPolymorphic() const noexcept override;

//private:

	CLinterOperator* m_pOperator = nullptr;
};
