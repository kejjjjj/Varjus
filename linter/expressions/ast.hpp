#pragma once

#include <memory>

#include "globalDefinitions.hpp"
#include "definitions.hpp"
#include "linter/punctuation.hpp"

class AbstractSyntaxTree;
using ASTNode = std::shared_ptr<AbstractSyntaxTree>;

class CLinterOperand;
class CLinterOperator;
class IRuntimeStructure;
struct COperandBase;

class AbstractSyntaxTree
{
	friend class AstToInstructionConverter;
public:


	AbstractSyntaxTree();
	virtual ~AbstractSyntaxTree();

	[[nodiscard]] virtual constexpr bool IsLeaf() const noexcept	 { return false; }
	[[nodiscard]] virtual constexpr bool IsOperator() const noexcept { return false; }
	[[nodiscard]] virtual constexpr bool IsPostfix() const noexcept  { return false; }
	[[nodiscard]] virtual constexpr bool IsVariable() const noexcept { return false; }
	[[nodiscard]] virtual constexpr bool IsFunction() const noexcept { return false; }
	[[nodiscard]] virtual constexpr bool IsConstant() const noexcept { return false; }
	[[nodiscard]] virtual constexpr bool IsSequence() const noexcept { return false; }

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

protected:
	[[nodiscard]] std::size_t GetLeftBranchDepth() const noexcept;

private:

	[[nodiscard]] static std::unique_ptr<AbstractSyntaxTree> GetLeaf(VectorOf<CLinterOperand*>& operands, VectorOf<CLinterOperator*>& operators);
	[[nodiscard]] static OperatorIterator FindLowestPriorityOperator(VectorOf<CLinterOperator*>& operators);
	
	void CreateRecursively(VectorOf<CLinterOperand*>& operands, VectorOf<CLinterOperator*>& operators);

};

class VariableASTNode final : public AbstractSyntaxTree
{
	friend class AstToInstructionConverter;
	NONCOPYABLE(VariableASTNode);
public:
	VariableASTNode(std::size_t variableIndex) : m_uIndex(variableIndex) {}

	[[nodiscard]] constexpr bool IsLeaf() const noexcept override { return true; }
	[[nodiscard]] constexpr bool IsVariable() const noexcept override { return true; }

	std::size_t m_uIndex{};

private:

};
class FunctionASTNode final : public AbstractSyntaxTree
{
	friend class AstToInstructionConverter;
	NONCOPYABLE(FunctionASTNode);
public:
	FunctionASTNode(std::size_t i) : m_uIndex(i) {}

	[[nodiscard]] constexpr bool IsLeaf() const noexcept override { return true; }
	[[nodiscard]] constexpr bool IsFunction() const noexcept override { return true; }

	std::size_t m_uIndex{};

private:

};
class ConstantASTNode final : public AbstractSyntaxTree
{
	NONCOPYABLE(ConstantASTNode);

public:

	ConstantASTNode(const std::string& data, EValueType datatype);
	~ConstantASTNode();

	[[nodiscard]] constexpr bool IsLeaf() const noexcept override { return true; }
	[[nodiscard]] constexpr bool IsConstant() const noexcept override { return true; }

	// contains the raw data for the constant
	std::string m_pConstant;
	EValueType m_eDataType{};
};

class OperatorASTNode : public AbstractSyntaxTree
{
	friend class AstToInstructionConverter;

public:
	OperatorASTNode() = default;
	OperatorASTNode(Punctuation punc) : m_ePunctuation(punc) {}
	virtual ~OperatorASTNode() = default;

	[[nodiscard]] constexpr bool IsOperator() const noexcept override { return true; }
	
	[[nodiscard]] virtual constexpr bool IsPostfix() const noexcept { return false; }
	[[nodiscard]] virtual constexpr bool IsSubscript() const noexcept { return false; }
	[[nodiscard]] virtual constexpr bool IsFunctionCall() const noexcept { return false; }

	[[nodiscard]] virtual constexpr bool IsSequence() const noexcept { return m_ePunctuation == p_comma; }

//private:
	Punctuation m_ePunctuation{};
};

class SubscriptASTNode : public OperatorASTNode
{
	NONCOPYABLE(SubscriptASTNode);
public:
	SubscriptASTNode(std::unique_ptr<AbstractSyntaxTree>&& expression) : m_pAST(std::move(expression)) {}
	[[nodiscard]] constexpr bool IsSubscript() const noexcept override { return true; }
	[[nodiscard]] constexpr bool IsPostfix() const noexcept { return true; }

	std::unique_ptr<AbstractSyntaxTree> m_pAST;
};

class FunctionCallASTNode : public OperatorASTNode
{
	NONCOPYABLE(FunctionCallASTNode);
public:
	FunctionCallASTNode(VectorOf<std::unique_ptr<AbstractSyntaxTree>>&& args) : m_oArguments(std::move(args)) {}
	[[nodiscard]] constexpr bool IsFunctionCall() const noexcept override { return true; }
	[[nodiscard]] constexpr bool IsPostfix() const noexcept { return true; }

	VectorOf<std::unique_ptr<AbstractSyntaxTree>> m_oArguments;
};