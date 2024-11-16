#pragma once

#include <memory>
#include <utility>

#include "globalDefinitions.hpp"
#include "definitions.hpp"
#include "linter/punctuation.hpp"

class AbstractSyntaxTree;
using ASTNode = std::shared_ptr<AbstractSyntaxTree>;
using ExpressionList = VectorOf<std::unique_ptr<AbstractSyntaxTree>>;
using UniqueAST = std::unique_ptr<AbstractSyntaxTree>;
template<typename A, typename B>
using KeyValue = std::pair<A, B>;

class CLinterOperand;
class CLinterOperator;
class IRuntimeStructure;
class CRuntimeFunction;
struct IOperand;

class VariableASTNode;
class ConstantASTNode;
class FunctionASTNode;
class ArrayASTNode;
class ObjectASTNode;
class TernaryASTNode;
class OperatorASTNode;

template<typename T>
concept Pointer = std::is_pointer_v<T> || std::is_reference_v<T>;

class AbstractSyntaxTree
{
public:

	AbstractSyntaxTree();
	virtual ~AbstractSyntaxTree();

	[[nodiscard]] virtual constexpr bool IsLeaf() const noexcept	 { return false; }
	[[nodiscard]] virtual constexpr bool IsOperator() const noexcept { return false; }
	[[nodiscard]] virtual constexpr bool IsPostfix() const noexcept  { return false; }
	[[nodiscard]] virtual constexpr bool IsVariable() const noexcept { return false; }
	[[nodiscard]] virtual constexpr bool IsFunction() const noexcept { return false; }
	[[nodiscard]] virtual constexpr bool IsConstant() const noexcept { return false; }
	[[nodiscard]] virtual constexpr bool IsArray() const noexcept    { return false; }
	[[nodiscard]] virtual constexpr bool IsObject() const noexcept { return false; }
	[[nodiscard]] virtual constexpr bool IsTernary() const noexcept { return false; }

	[[nodiscard]] virtual constexpr bool IsSequence() const noexcept { return false; }

	[[nodiscard]] virtual constexpr const OperatorASTNode* GetOperator() const noexcept { return nullptr; }
	[[nodiscard]] virtual constexpr const VariableASTNode* GetVariable() const noexcept { return nullptr; }
	[[nodiscard]] virtual constexpr const ConstantASTNode* GetConstant() const noexcept { return nullptr; }
	[[nodiscard]] virtual constexpr const FunctionASTNode* GetFunction() const noexcept { return nullptr; }
	[[nodiscard]] virtual constexpr const ArrayASTNode* GetArray() const noexcept { return nullptr; }
	[[nodiscard]] virtual constexpr const ObjectASTNode* GetObject() const noexcept { return nullptr; }
	[[nodiscard]] virtual constexpr const TernaryASTNode* GetTernary() const noexcept { return nullptr; }

	template<Pointer T>
	[[nodiscard]] inline constexpr T As() const noexcept {
		return dynamic_cast<T>(this); 
	}

	template<Pointer T>
	[[nodiscard]] inline constexpr T As() noexcept {
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

	[[nodiscard]] constexpr const VariableASTNode* GetVariable() const noexcept override { return this; }

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

	[[nodiscard]] constexpr const FunctionASTNode* GetFunction() const noexcept override { return this; }

	std::size_t m_uIndex{};
};
class ConstantASTNode final : public AbstractSyntaxTree
{
	NONCOPYABLE(ConstantASTNode);

public:

	ConstantASTNode(const std::string& data, EValueType datatype);
	~ConstantASTNode();

	[[nodiscard]] constexpr bool IsLeaf() const noexcept override { return true; }
	[[nodiscard]] constexpr bool IsConstant() const noexcept override { return true; }
	[[nodiscard]] constexpr const ConstantASTNode* GetConstant() const noexcept override { return this; }


	// contains the raw data for the constant
	std::string m_pConstant;
	EValueType m_eDataType{};
};
class ArrayASTNode final : public AbstractSyntaxTree
{
	NONCOPYABLE(ArrayASTNode);

public:

	ArrayASTNode(ExpressionList&& expressions);
	~ArrayASTNode();

	[[nodiscard]] constexpr bool IsLeaf() const noexcept override { return true; }
	[[nodiscard]] constexpr bool IsArray() const noexcept override { return true; }

	[[nodiscard]] constexpr const ArrayASTNode* GetArray() const noexcept override { return this; }

	ExpressionList m_oExpressions;
};

class ObjectASTNode final : public AbstractSyntaxTree
{
	NONCOPYABLE(ObjectASTNode);

public:

	ObjectASTNode(VectorOf<KeyValue<std::size_t, UniqueAST>>&& expressions);
	~ObjectASTNode();

	[[nodiscard]] constexpr bool IsLeaf() const noexcept override { return true; }
	[[nodiscard]] constexpr bool IsObject() const noexcept override { return true; }

	[[nodiscard]] constexpr const ObjectASTNode* GetObject() const noexcept override { return this; }

	VectorOf<KeyValue<std::size_t, UniqueAST>> m_oAttributes;
};

class TernaryASTNode final : public AbstractSyntaxTree
{
	NONCOPYABLE(TernaryASTNode);

public:

	TernaryASTNode(struct CTernaryOperand* operand);
	~TernaryASTNode();

	[[nodiscard]] constexpr bool IsLeaf() const noexcept override { return true; }
	[[nodiscard]] constexpr bool IsTernary() const noexcept override { return true; }

	[[nodiscard]] constexpr const TernaryASTNode* GetTernary() const noexcept override { return this; }

	UniqueAST m_pOperand;
	UniqueAST m_pTrue;
	UniqueAST m_pFalse;
};

class OperatorASTNode : public AbstractSyntaxTree
{
	friend class AstToInstructionConverter;

public:
	OperatorASTNode() = default;
	OperatorASTNode(Punctuation punc) : m_ePunctuation(punc) {}
	virtual ~OperatorASTNode() = default;

	[[nodiscard]] constexpr const OperatorASTNode* GetOperator() const noexcept override { return this; }

	[[nodiscard]] constexpr bool IsOperator() const noexcept override { return true; }
	
	[[nodiscard]] virtual constexpr bool IsPostfix() const noexcept { return false; }
	[[nodiscard]] virtual constexpr bool IsMemberAccess() const noexcept { return false; }
	[[nodiscard]] virtual constexpr bool IsSubscript() const noexcept { return false; }
	[[nodiscard]] virtual constexpr bool IsFunctionCall() const noexcept { return false; }

	[[nodiscard]] virtual constexpr bool IsSequence() const noexcept { return m_ePunctuation == p_comma; }

//private:
	Punctuation m_ePunctuation{};
};

class MemberAccessASTNode : public OperatorASTNode
{
	NONCOPYABLE(MemberAccessASTNode);
public:
	MemberAccessASTNode(std::size_t globalMemberIndex)
		: m_uGlobalMemberIndex(globalMemberIndex) {}

	[[nodiscard]] constexpr bool IsMemberAccess() const noexcept override { return true; }
	[[nodiscard]] constexpr bool IsPostfix() const noexcept { return true; }

	std::size_t m_uGlobalMemberIndex;
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