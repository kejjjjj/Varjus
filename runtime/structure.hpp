#pragma once
#include <vector>
#include <memory>
#include <string>
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
	st_while,
	st_return,
};

class CRuntimeFunction;
class CFunction;
class IValue;
class OperatorASTNode;
class MemberAccessASTNode;
class SubscriptASTNode;
class FunctionCallASTNode;

struct CFunctionBlock;

template<typename T>
using VectorOf = std::vector<T>;

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
		[[maybe_unused]] CFunction* const thisFunction, [[maybe_unused]] VectorOf<IValue*>& args) { return nullptr; };

protected:
};

using InstructionSequence = VectorOf<std::unique_ptr<IRuntimeStructure>>;

using RuntimeBlock = std::unique_ptr<IRuntimeStructure>;
using RuntimeFunction = std::unique_ptr<CRuntimeFunction>;

using FunctionArgument = RuntimeBlock;
using FunctionArguments = VectorOf<FunctionArgument>;
using ExpressionList = VectorOf<std::unique_ptr<AbstractSyntaxTree>>;


// contains more than one instruction
class IRuntimeStructureSequence : public IRuntimeStructure
{
	NONCOPYABLE(IRuntimeStructureSequence);
public:
	IRuntimeStructureSequence(InstructionSequence&& insns);
	~IRuntimeStructureSequence();

	[[nodiscard]] IValue* ExecuteBlock([[maybe_unused]] CFunction* const thisFunction);

protected:
	InstructionSequence m_oInstructions;
};

class CRuntimeFunction final : public IRuntimeStructureSequence
{
	NONCOPYABLE(CRuntimeFunction);

public:
	CRuntimeFunction(CFunctionBlock& linterFunction);
	~CRuntimeFunction();

	[[nodiscard]] constexpr auto& GetName() const noexcept { return m_sName; }
	[[maybe_unused]] IValue* Execute(CFunction* const thisFunction, VectorOf<IValue*>& args) override;
protected:

	[[nodiscard]] constexpr EStructureType Type() const noexcept { return st_function; };

	std::string m_sName;
	std::size_t m_uNumParameters{ 0u };
	std::size_t m_uNumVariables{ 0u };
};


class CRuntimeExpression final : public IRuntimeStructure
{
	NONCOPYABLE(CRuntimeExpression);
	friend class CRuntimeReturnStatement;

public:
	CRuntimeExpression(std::unique_ptr<AbstractSyntaxTree>&& ast);
	~CRuntimeExpression();

	[[maybe_unused]] IValue* Execute(CFunction* const thisFunction) override;
	[[nodiscard]] IValue* Evaluate(CFunction* const thisFunction);

protected:
	[[nodiscard]] constexpr EStructureType Type() const noexcept { return st_expression;};

private:
	[[nodiscard]] static IValue* Evaluate(CFunction* const thisFunction, const AbstractSyntaxTree* node);
	[[nodiscard]] static IValue* EvaluateLeaf(CFunction* const thisFunction, const AbstractSyntaxTree* node);
	[[nodiscard]] static IValue* EvaluatePostfix(CFunction* const thisFunction, const OperatorASTNode* node);
	[[nodiscard]] static IValue* EvaluateSequence(CFunction* const thisFunction, const AbstractSyntaxTree* node);

	[[nodiscard]] static IValue* EvaluateMemberAccess(IValue* operand, const MemberAccessASTNode* node);
	[[nodiscard]] static IValue* EvaluateSubscript(CFunction* const thisFunction, IValue* operand, const SubscriptASTNode* node);
	[[nodiscard]] static IValue* EvaluateFunctionCall(CFunction* const thisFunction, IValue* operand, const FunctionCallASTNode* node);

	[[nodiscard]] static VectorOf<IValue*> EvaluateList(CFunction* const thisFunction, const ExpressionList& list);

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
	[[nodiscard]] constexpr EStructureType Type() const noexcept { return st_conditional; };

private:

	CRuntimeConditionalStatement* SeekLastBlock();


	std::unique_ptr<CRuntimeExpression> m_pCondition;
	std::unique_ptr<CRuntimeConditionalStatement> m_pNext; //else (if)
};

class CRuntimeWhileStatement final : public IRuntimeStructureSequence
{
	NONCOPYABLE(CRuntimeWhileStatement);
public:
	CRuntimeWhileStatement(std::unique_ptr<AbstractSyntaxTree>&& condition, InstructionSequence&& insns);
	~CRuntimeWhileStatement();

	[[maybe_unused]] IValue* Execute(CFunction* const thisFunction) override;

protected:
	[[nodiscard]] constexpr EStructureType Type() const noexcept { return st_while; };

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
	[[nodiscard]] constexpr EStructureType Type() const noexcept { return st_return; };

private:
	std::unique_ptr<AbstractSyntaxTree> m_pAST;
};
class IRuntimeBlock
{
public:
	virtual ~IRuntimeBlock() = default;

protected:
	virtual RuntimeBlock ToRuntimeObject() const = 0;
};

class CFileRuntimeData final
{
	friend class CProgramRuntime;
	NONCOPYABLE(CFileRuntimeData)
public:
	CFileRuntimeData() = default;
	constexpr void AddFunction(RuntimeFunction&& func) { m_oFunctions.emplace_back(std::move(func)); }

	[[nodiscard]] CRuntimeFunction* FindFunction(const std::string& v) const;
	[[nodiscard]] size_t FindFunctionIndex(const std::string& v) const;

private:
	VectorOf<RuntimeFunction> m_oFunctions;
};

#pragma pack(pop)