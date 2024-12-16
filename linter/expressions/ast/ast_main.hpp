#pragma once

#include <memory>
#include <utility>

#include "globalDefinitions.hpp"
#include "linter/expressions/definitions.hpp"
#include "linter/punctuation.hpp"
#include "linter/modules/references.hpp"

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
class LambdaASTNode;
class OperatorASTNode;

template<typename T>
concept Pointer = std::is_pointer_v<T> || std::is_reference_v<T>;
using RuntimeFunction = std::unique_ptr<CRuntimeFunction>;
using ElementIndex = std::size_t;

class AbstractSyntaxTree
{
public:
	AbstractSyntaxTree() = default;
	AbstractSyntaxTree(const CodePosition& pos) : m_oApproximatePosition(pos) {}
	virtual ~AbstractSyntaxTree();

	[[nodiscard]] const CodePosition& GetCodePosition() const noexcept { return m_oApproximatePosition; }

	[[nodiscard]] virtual constexpr bool IsLeaf() const noexcept { return false; }
	[[nodiscard]] virtual constexpr bool IsOperator() const noexcept { return false; }
	[[nodiscard]] virtual constexpr bool IsPostfix() const noexcept { return false; }
	[[nodiscard]] virtual constexpr bool IsUnary() const noexcept { return false; }
	[[nodiscard]] virtual constexpr bool IsVariable() const noexcept { return false; }
	[[nodiscard]] virtual constexpr bool IsFunction() const noexcept { return false; }
	[[nodiscard]] virtual constexpr bool IsConstant() const noexcept { return false; }
	[[nodiscard]] virtual constexpr bool IsArray() const noexcept { return false; }
	[[nodiscard]] virtual constexpr bool IsObject() const noexcept { return false; }
	[[nodiscard]] virtual constexpr bool IsTernary() const noexcept { return false; }
	[[nodiscard]] virtual constexpr bool IsLambda() const noexcept { return false; }

	[[nodiscard]] virtual constexpr bool IsSequence() const noexcept { return false; }

	[[nodiscard]] virtual constexpr const OperatorASTNode* GetOperator() const noexcept { return nullptr; }
	[[nodiscard]] virtual constexpr const VariableASTNode* GetVariable() const noexcept { return nullptr; }
	[[nodiscard]] virtual constexpr const ConstantASTNode* GetConstant() const noexcept { return nullptr; }
	[[nodiscard]] virtual constexpr const FunctionASTNode* GetFunction() const noexcept { return nullptr; }
	[[nodiscard]] virtual constexpr const ArrayASTNode* GetArray() const noexcept { return nullptr; }
	[[nodiscard]] virtual constexpr const ObjectASTNode* GetObject() const noexcept { return nullptr; }
	[[nodiscard]] virtual constexpr const TernaryASTNode* GetTernary() const noexcept { return nullptr; }
	[[nodiscard]] virtual constexpr const LambdaASTNode* GetLambda() const noexcept { return nullptr; }

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
	CodePosition m_oApproximatePosition;
};

struct CLinterVariable;
struct CLinterFunction;

class VariableASTNode final : public AbstractSyntaxTree, public CCrossModuleReference
{
	friend class AstToInstructionConverter;
	NONCOPYABLE(VariableASTNode);
public:
	VariableASTNode(const CodePosition& pos, CLinterVariable* const var);

	[[nodiscard]] constexpr bool IsLeaf() const noexcept override { return true; }
	[[nodiscard]] constexpr bool IsVariable() const noexcept override { return true; }

	[[nodiscard]] constexpr const VariableASTNode* GetVariable() const noexcept override { return this; }

	bool m_bGlobalVariable{ false };
private:

};
class FunctionASTNode final : public AbstractSyntaxTree, public CCrossModuleReference
{
	friend class AstToInstructionConverter;
	NONCOPYABLE(FunctionASTNode);
public:
	FunctionASTNode(const CodePosition& pos, CLinterFunction* const func);

	[[nodiscard]] constexpr bool IsLeaf() const noexcept override { return true; }
	[[nodiscard]] constexpr bool IsFunction() const noexcept override { return true; }

	[[nodiscard]] constexpr const FunctionASTNode* GetFunction() const noexcept override { return this; }
};
class ConstantASTNode final : public AbstractSyntaxTree
{
	NONCOPYABLE(ConstantASTNode);

public:

	ConstantASTNode(const CodePosition& pos, const std::string& data, EValueType datatype);
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

	ArrayASTNode(const CodePosition& pos, ExpressionList&& expressions);
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

	ObjectASTNode(const CodePosition& pos, VectorOf<KeyValue<std::size_t, UniqueAST>>&& expressions);
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

	TernaryASTNode(const CodePosition& pos, struct CTernaryOperand* operand);
	~TernaryASTNode();

	[[nodiscard]] constexpr bool IsLeaf() const noexcept override { return true; }
	[[nodiscard]] constexpr bool IsTernary() const noexcept override { return true; }

	[[nodiscard]] constexpr const TernaryASTNode* GetTernary() const noexcept override { return this; }

	UniqueAST m_pOperand;
	UniqueAST m_pTrue;
	UniqueAST m_pFalse;
};
struct CCrossModuleReference;

class LambdaASTNode final : public AbstractSyntaxTree
{
	NONCOPYABLE(LambdaASTNode);

public:

	LambdaASTNode(const CodePosition& pos, RuntimeFunction&& operand, VectorOf<CCrossModuleReference>&& captures);
	~LambdaASTNode();

	[[nodiscard]] constexpr bool IsLeaf() const noexcept override { return true; }
	[[nodiscard]] constexpr bool IsLambda() const noexcept override { return true; }

	[[nodiscard]] constexpr const LambdaASTNode* GetLambda() const noexcept override { return this; }

	RuntimeFunction m_pLambda;
	VectorOf<CCrossModuleReference> m_oVariableCaptures;
};

class PostfixASTNode;
class UnaryASTNode;

class OperatorASTNode : public AbstractSyntaxTree
{
public:
	OperatorASTNode() = default;
	OperatorASTNode(const CodePosition& pos) : AbstractSyntaxTree(pos) {}
	OperatorASTNode(const CodePosition& pos, Punctuation punc) : AbstractSyntaxTree(pos), m_ePunctuation(punc) {}
	virtual ~OperatorASTNode() = default;

	[[nodiscard]] constexpr const OperatorASTNode* GetOperator() const noexcept override { return this; }
	[[nodiscard]] constexpr bool IsOperator() const noexcept override { return true; }

	[[nodiscard]] virtual constexpr bool IsPostfix() const noexcept { return false; }
	[[nodiscard]] virtual constexpr bool IsUnary() const noexcept { return false; }

	[[nodiscard]] virtual constexpr const PostfixASTNode* GetPostfix() const noexcept { return nullptr; }
	[[nodiscard]] virtual constexpr const UnaryASTNode* GetUnary() const noexcept { return nullptr; }


	[[nodiscard]] virtual constexpr bool IsSequence() const noexcept { return m_ePunctuation == p_comma; }

	//private:
	Punctuation m_ePunctuation{};
};

