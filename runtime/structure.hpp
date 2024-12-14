#pragma once
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include "globalDefinitions.hpp"

/***********************************************************************
 > represents the runtime execution structure
***********************************************************************/
class AbstractSyntaxTree;

enum EStructureType
{
	st_function,
	st_expression,
	st_conditional,
	st_for,
	st_while,
	st_return,
	st_loop_control,
	st_try_catch,
	st_throw
};
enum EExecutionControl : char
{
	lc_null,
	lc_break,
	lc_continue,
};
class CRuntimeFunction;
class CFunction;
class IValue;
class OperatorASTNode;
class PostfixASTNode;
class UnaryASTNode;
class MemberAccessASTNode;
class SubscriptASTNode;
class FunctionCallASTNode;
class TernaryASTNode;
class CVariable;

struct CFunctionBlock;

template<typename T>
using VectorOf = std::vector<T>;
using ArgumentIndex = std::size_t;
using VariableIndex = std::size_t;
using VariableCaptures = std::unordered_map<VariableIndex, CVariable*>;

#pragma pack(push)
#pragma warning(disable : 4266)


class IRuntimeStructure
{
	NONCOPYABLE(IRuntimeStructure);
public:
	IRuntimeStructure();
	virtual ~IRuntimeStructure();

	[[nodiscard]] virtual constexpr EStructureType Type() const noexcept = 0;
	[[nodiscard]] virtual IValue* Execute([[maybe_unused]] CFunction* const thisFunction) { return nullptr; };
	[[nodiscard]] virtual IValue* Execute(
		[[maybe_unused]] std::size_t ownerModule,
		[[maybe_unused]] CFunction* const thisFunction, 
		[[maybe_unused]] VectorOf<IValue*>& args,
		[[maybe_unused]] const VariableCaptures& captures) { return nullptr; };

	[[nodiscard]] static EExecutionControl ToControlStatement(const IValue* rv);


protected:
};

using InstructionSequence = VectorOf<std::unique_ptr<IRuntimeStructure>>;

using RuntimeBlock = std::unique_ptr<IRuntimeStructure>;
using RuntimeFunction = std::unique_ptr<CRuntimeFunction>;

using FunctionArgument = RuntimeBlock;
using FunctionArguments = VectorOf<FunctionArgument>;
using ExpressionList = VectorOf<std::unique_ptr<AbstractSyntaxTree>>;

template<typename T>
using VectorOf = std::vector<T>;

template<typename A, typename B>
using KeyValue = std::pair<A, B>;

using ElementIndex = std::size_t;
using UniqueAST = std::unique_ptr<AbstractSyntaxTree>;
using ObjectInitializer = VectorOf<KeyValue<ElementIndex, IValue*>>;
using ObjectInitializerData = VectorOf<KeyValue<std::size_t, UniqueAST>>;

// contains more than one instruction
class IRuntimeStructureSequence : public IRuntimeStructure
{
	NONCOPYABLE(IRuntimeStructureSequence);
public:
	IRuntimeStructureSequence(InstructionSequence&& insns);
	~IRuntimeStructureSequence();

	[[nodiscard]] IValue* ExecuteBlock([[maybe_unused]] CFunction* const thisFunction);
	
	[[nodiscard]] auto NumInstructions() const noexcept {
		return m_oInstructions.size();
	}
	[[nodiscard]] auto& GetInstruction(std::size_t idx) const noexcept {
		return m_oInstructions[idx];
	}
protected:
	InstructionSequence m_oInstructions;
};

class CRuntimeFunction final : public IRuntimeStructureSequence
{
	friend class CFunction;
	NONCOPYABLE(CRuntimeFunction);

public:
	CRuntimeFunction(ElementIndex moduleIndex, CFunctionBlock& linterFunction,
		VectorOf<VariableIndex>&& args,
		VectorOf<VariableIndex>&& variableIndices);
	~CRuntimeFunction();

	[[nodiscard]] constexpr auto& GetName() const noexcept { return m_sName; }
	[[nodiscard]] constexpr auto& GetModuleIndex() const noexcept { return m_uModuleIndex; }

	[[maybe_unused]] IValue* Execute(std::size_t ownerModule, CFunction* const thisFunction, VectorOf<IValue*>& args,
		const VariableCaptures& captures) override;
protected:

	[[nodiscard]] constexpr EStructureType Type() const noexcept override { return st_function; };

	ElementIndex m_uModuleIndex{ 0u };

	std::string m_sName;
	std::size_t m_uNumParameters{ 0u };
	std::size_t m_uNumVariables{ 0u };
	VectorOf<VariableIndex> m_oArgumentIndices;
	VectorOf<VariableIndex> m_oVariableIndices;
};


class CRuntimeExpression final : public IRuntimeStructure
{
	NONCOPYABLE(CRuntimeExpression);
	friend class CRuntimeReturnStatement;
	friend class CRuntimeThrowStatement;

public:
	CRuntimeExpression(std::unique_ptr<AbstractSyntaxTree>&& ast);
	~CRuntimeExpression();

	[[maybe_unused]] IValue* Execute(CFunction* const thisFunction) override;
	[[nodiscard]] IValue* Evaluate(CFunction* const thisFunction);
	[[nodiscard]] inline bool HasAST() { return !!m_pAST.get(); }
protected:
	[[nodiscard]] constexpr EStructureType Type() const noexcept override { return st_expression;};

private:
	[[nodiscard]] static IValue* Evaluate(CFunction* const thisFunction, const AbstractSyntaxTree* node);
	[[nodiscard]] static IValue* EvaluateLeaf(CFunction* const thisFunction, const AbstractSyntaxTree* node);
	[[nodiscard]] static IValue* EvaluatePostfix(CFunction* const thisFunction, const PostfixASTNode* node);
	[[nodiscard]] static IValue* EvaluateUnary(CFunction* const thisFunction, const UnaryASTNode* node);

	[[nodiscard]] static IValue* EvaluateSequence(CFunction* const thisFunction, const AbstractSyntaxTree* node);
	[[nodiscard]] static IValue* EvaluateTernary(CFunction* const thisFunction, const TernaryASTNode* node);

	[[nodiscard]] static IValue* EvaluateMemberAccess(IValue* operand, const MemberAccessASTNode* node);
	[[nodiscard]] static IValue* EvaluateSubscript(CFunction* const thisFunction, IValue* operand, const SubscriptASTNode* node);
	[[nodiscard]] static IValue* EvaluateFunctionCall(CFunction* const thisFunction, IValue* operand, const FunctionCallASTNode* node);

	[[nodiscard]] static VectorOf<IValue*> EvaluateList(CFunction* const thisFunction, const ExpressionList& list);
	[[nodiscard]] static ObjectInitializer EvaluateObject(CFunction* const thisFunction, const ObjectInitializerData& obj);
	std::unique_ptr<AbstractSyntaxTree> m_pAST;

};

class CRuntimeConditionalStatement final : public IRuntimeStructureSequence
{
	NONCOPYABLE(CRuntimeConditionalStatement);
	friend class CElseStatementLinter; // so that it can add the chaining
public:
	CRuntimeConditionalStatement(std::unique_ptr<AbstractSyntaxTree>&& condition, InstructionSequence&& insns);
	~CRuntimeConditionalStatement();

	[[maybe_unused]] IValue* Execute(CFunction* const thisFunction) override;

protected:
	[[nodiscard]] constexpr EStructureType Type() const noexcept override { return st_conditional; };

private:

	CRuntimeConditionalStatement* SeekLastBlock();


	std::unique_ptr<CRuntimeExpression> m_pCondition;
	std::unique_ptr<CRuntimeConditionalStatement> m_pNext; //else (if)
};

class CRuntimeForStatement final : public IRuntimeStructureSequence
{
	NONCOPYABLE(CRuntimeForStatement);
public:
	CRuntimeForStatement(
		std::unique_ptr<AbstractSyntaxTree>&& init,
		std::unique_ptr<AbstractSyntaxTree>&& cond,
		std::unique_ptr<AbstractSyntaxTree>&& endExpr, InstructionSequence&& insns);
	~CRuntimeForStatement();

	[[maybe_unused]] IValue* Execute(CFunction* const thisFunction) override;

protected:
	[[nodiscard]] constexpr EStructureType Type() const noexcept override { return st_for; };

private:
	std::unique_ptr<CRuntimeExpression> m_pInitializer;
	std::unique_ptr<CRuntimeExpression> m_pCondition;
	std::unique_ptr<CRuntimeExpression> m_pOnEnd;
};

class CRuntimeWhileStatement final : public IRuntimeStructureSequence
{
	NONCOPYABLE(CRuntimeWhileStatement);
public:
	CRuntimeWhileStatement(std::unique_ptr<AbstractSyntaxTree>&& condition, InstructionSequence&& insns);
	~CRuntimeWhileStatement();

	[[maybe_unused]] IValue* Execute(CFunction* const thisFunction) override;

protected:
	[[nodiscard]] constexpr EStructureType Type() const noexcept override { return st_while; };

private:
	std::unique_ptr<CRuntimeExpression> m_pCondition;
};
class CRuntimeReturnStatement final : public IRuntimeStructure
{
	NONCOPYABLE(CRuntimeReturnStatement);
public:
	CRuntimeReturnStatement(std::unique_ptr<AbstractSyntaxTree>&& condition);
	~CRuntimeReturnStatement();

	[[maybe_unused]] IValue* Execute(CFunction* const thisFunction) override;

protected:
	[[nodiscard]] constexpr EStructureType Type() const noexcept override { return st_return; };

private:
	std::unique_ptr<AbstractSyntaxTree> m_pAST;
};

class CRuntimeTryCatchStatement final : public IRuntimeStructure
{
	NONCOPYABLE(CRuntimeTryCatchStatement);
public:
	CRuntimeTryCatchStatement(VariableIndex catchVariable, InstructionSequence&& tryBlock, InstructionSequence&& catchBlock);
	~CRuntimeTryCatchStatement();

	[[maybe_unused]] IValue* Execute(CFunction* const thisFunction) override;

protected:
	[[nodiscard]] constexpr EStructureType Type() const noexcept override { return st_try_catch; };
private:
	[[maybe_unused]] IValue* ExecuteCatchBlock(CFunction* const thisFunction, IValue* ex);

	VariableIndex m_uCatchVariableIndex{};
	InstructionSequence m_oTryInstructions;
	InstructionSequence m_oCatchInstructions;
};

class CRuntimeThrowStatement final : public IRuntimeStructure
{
	NONCOPYABLE(CRuntimeThrowStatement);
public:
	CRuntimeThrowStatement(std::unique_ptr<AbstractSyntaxTree>&& condition);
	~CRuntimeThrowStatement();

	[[maybe_unused]] IValue* Execute(CFunction* const thisFunction) override;

protected:
	[[nodiscard]] constexpr EStructureType Type() const noexcept override { return st_throw; };

private:
	std::unique_ptr<AbstractSyntaxTree> m_pAST;
};

class CRuntimeLoopControlStatement final : public IRuntimeStructure {
	NONCOPYABLE(CRuntimeLoopControlStatement);
public:
	CRuntimeLoopControlStatement(EExecutionControl c) : m_eCtrl(c){}
	
	[[maybe_unused]] IValue* Execute(CFunction* const thisFunction) override;

protected:
	[[nodiscard]] constexpr EStructureType Type() const noexcept override { return st_loop_control; };
private:
	EExecutionControl m_eCtrl{ lc_null };
};

class IRuntimeBlock
{
public:
	virtual ~IRuntimeBlock() = default;

protected:
	virtual RuntimeBlock ToRuntimeObject() const = 0;
};
#pragma pack(pop)