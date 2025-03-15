#pragma once

#include <memory>
#include <utility>
#include <stdexcept>
#include <variant>

#include "varjus_api/internal/globalDefinitions.hpp"
#include "linter/expressions/definitions.hpp"
#include "linter/punctuation.hpp"
#include "varjus_api/types/internal/references.hpp"

class AbstractSyntaxTree;
using ASTNode = std::shared_ptr<AbstractSyntaxTree>;
using ExpressionList = VectorOf<ASTNode>;
using UniqueAST = std::unique_ptr<AbstractSyntaxTree>;
template<typename A, typename B>
using KeyValue = std::pair<A, B>;

class CLinterOperand;
class CLinterOperator;
class IRuntimeStructure;
class CRuntimeFunctionBase;

struct IOperand;

class VariableASTNode;
class ConstantASTNode;
class FunctionASTNode;
class ArrayASTNode;
class ObjectASTNode;
class TernaryASTNode;
class LambdaASTNode;
class FmtStringASTNode;
class OperatorASTNode;

class CMemory;
class IConstEvalValue;

template<typename T>
concept Pointer = std::is_pointer_v<T> || std::is_reference_v<T>;
using RuntimeFunction = std::unique_ptr<CRuntimeFunctionBase>;
using ElementIndex = std::size_t;

using Operands = VectorOf<CLinterOperand*>;
using Operators = VectorOf<CLinterOperator*>;

class AbstractSyntaxTree : public std::enable_shared_from_this<AbstractSyntaxTree>
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
	[[nodiscard]] virtual constexpr bool IsFmtString() const noexcept { return false; }
	[[nodiscard]] virtual constexpr bool IsLambda() const noexcept { return false; }
	[[nodiscard]] virtual constexpr bool IsSequence() const noexcept { return false; }

	[[nodiscard]] virtual constexpr const OperatorASTNode* GetOperator() const noexcept { return nullptr; }
	[[nodiscard]] virtual constexpr const VariableASTNode* GetVariable() const noexcept { return nullptr; }
	[[nodiscard]] virtual constexpr VariableASTNode* GetVariable() noexcept { return nullptr; }

	[[nodiscard]] virtual constexpr const ConstantASTNode* GetConstant() const noexcept { return nullptr; }
	[[nodiscard]] virtual constexpr const FunctionASTNode* GetFunction() const noexcept { return nullptr; }
	[[nodiscard]] virtual constexpr const ArrayASTNode* GetArray() const noexcept { return nullptr; }
	[[nodiscard]] virtual constexpr const ObjectASTNode* GetObject() const noexcept { return nullptr; }
	[[nodiscard]] virtual constexpr const TernaryASTNode* GetTernary() const noexcept { return nullptr; }
	[[nodiscard]] virtual constexpr const LambdaASTNode* GetLambda() const noexcept { return nullptr; }
	[[nodiscard]] virtual constexpr const FmtStringASTNode* GetFmtString() const noexcept { return nullptr; }

	ASTNode left;
	ASTNode right;

public:
	[[nodiscard]] static ASTNode CreateAST(CMemory* const owner, Operands& operands, Operators& operators);

private:
	[[nodiscard]] static ASTNode GetLeaf(Operands& operands, Operators& operators);
	[[nodiscard]] static OperatorIterator FindLowestPriorityOperator(Operators& operators);

	static void CreateRecursively(ASTNode& _this, CMemory* const owner, Operands& operands, Operators& operators);
	static void CreateTernary(ASTNode& _this, CMemory* const owner,
		Operands& lhs_operands, Operators& lhs_operators, Operands& rhs_operands, Operators& rhs_operators);

	[[nodiscard]] bool IsAssignment() const noexcept;
	
	void CheckConstness(CMemory* const owner) const;
	void CheckSelfCapture(CMemory* const owner);

	[[nodiscard]] bool IsSelfReferencingCapture(const AbstractSyntaxTree* lhs, const AbstractSyntaxTree* rhs);

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
	~VariableASTNode();
	[[nodiscard]] constexpr bool IsLeaf() const noexcept override { return true; }
	[[nodiscard]] constexpr bool IsVariable() const noexcept override { return true; }

	[[nodiscard]] constexpr const VariableASTNode* GetVariable() const noexcept override { return this; }
	[[nodiscard]] constexpr VariableASTNode* GetVariable() noexcept override { return this; }

	bool m_bGlobalVariable{ false };
	bool m_bSelfCapturing{ false };
	bool m_bIsConst{ false };
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
	ConstantASTNode(ConstantASTNode&& other) = default;
	ConstantASTNode(const CodePosition& pos, const VarjusString& data, EValueType datatype);
	~ConstantASTNode();

	[[nodiscard]] constexpr bool IsLeaf() const noexcept override { return true; }
	[[nodiscard]] constexpr bool IsConstant() const noexcept override { return true; }
	[[nodiscard]] constexpr const ConstantASTNode* GetConstant() const noexcept override { return this; }

	// contains the raw data for the constant
	VarjusString m_pConstant;
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

	ObjectASTNode(const CodePosition& pos, VectorOf<KeyValue<std::size_t, ASTNode >> && expressions);
	~ObjectASTNode();

	[[nodiscard]] constexpr bool IsLeaf() const noexcept override { return true; }
	[[nodiscard]] constexpr bool IsObject() const noexcept override { return true; }

	[[nodiscard]] constexpr const ObjectASTNode* GetObject() const noexcept override { return this; }

	VectorOf<KeyValue<std::size_t, ASTNode>> m_oAttributes;
};

class TernaryASTNode final : public AbstractSyntaxTree
{
	NONCOPYABLE(TernaryASTNode);

public:

	TernaryASTNode(const CodePosition& pos, ASTNode& value, ASTNode& m_true, ASTNode& m_false);
	~TernaryASTNode();

	[[nodiscard]] constexpr bool IsLeaf() const noexcept override { return true; }
	[[nodiscard]] constexpr bool IsTernary() const noexcept override { return true; }

	[[nodiscard]] constexpr const TernaryASTNode* GetTernary() const noexcept override { return this; }

	ASTNode m_pOperand;
	ASTNode m_pTrue;
	ASTNode m_pFalse;
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

struct FmtStringAST {
	enum FmtNodeASTType { TEXT, PLACEHOLDER } type;
	std::variant<VarjusString, ASTNode> value; // Raw text for TEXT, variable name for PLACEHOLDER

	explicit FmtStringAST(const VarjusString& raw) : type(TEXT), value(raw) {}
	explicit FmtStringAST(ASTNode&& raw) : type(PLACEHOLDER), value(std::move(raw)) {}
};

class FmtStringASTNode final : public AbstractSyntaxTree
{
	NONCOPYABLE(FmtStringASTNode);

public:

	FmtStringASTNode(const CodePosition& pos, VectorOf<FmtStringAST>&& nodes);
	~FmtStringASTNode();

	[[nodiscard]] constexpr bool IsLeaf() const noexcept override { return true; }
	[[nodiscard]] constexpr bool IsFmtString() const noexcept override { return true; }

	[[nodiscard]] constexpr const FmtStringASTNode* GetFmtString() const noexcept override { return this; }


	VectorOf<FmtStringAST> m_oNodes;
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

