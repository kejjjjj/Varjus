#pragma once

#include "globalDefinitions.hpp"
#include "api/types/internal/references.hpp"
#include "api/types/internal/methods.hpp"

#include <vector>
#include <memory>
#include <string>
#include <unordered_map>

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
class FmtStringASTNode;
class CVariable;
class CRuntimeModule;

struct CFunctionBlock;

template<typename T>
using VectorOf = std::vector<T>;
using ArgumentIndex = std::size_t;
using VariableIndex = std::size_t;
using VariableCaptures = std::unordered_map<CCrossModuleReference, CVariable*, CCrossModuleReferenceHasher>;

#pragma pack(push)
#pragma warning(disable : 4266)

struct CRuntimeContext
{
	CRuntimeModule* m_pModule;
	CFunction* m_pFunction;
};

class IRuntimeStructure
{
	NONCOPYABLE(IRuntimeStructure);
public:
	IRuntimeStructure();
	virtual ~IRuntimeStructure();

	[[nodiscard]] virtual constexpr EStructureType Type() const noexcept = 0;

	[[nodiscard]] virtual IValue* Execute(
		[[maybe_unused]] CRuntimeContext* const ctx) { return nullptr; };

	[[nodiscard]] virtual IValue* Execute(
		[[maybe_unused]] CRuntimeContext* const ctx,
		[[maybe_unused]] IValue* _this,
		[[maybe_unused]] VectorOf<IValue*>& args,
		[[maybe_unused]] const VariableCaptures& captures) { return nullptr; };

	[[nodiscard]] static EExecutionControl ToControlStatement(const IValue* rv);


protected:
};

using UniqueAST = std::unique_ptr<AbstractSyntaxTree>;
using ASTNode = std::shared_ptr<AbstractSyntaxTree>;

using InstructionSequence = VectorOf<std::unique_ptr<IRuntimeStructure>>;

using RuntimeBlock = std::unique_ptr<IRuntimeStructure>;
using RuntimeFunction = std::unique_ptr<CRuntimeFunction>;

using FunctionArgument = RuntimeBlock;
using FunctionArguments = VectorOf<FunctionArgument>;
using ExpressionList = VectorOf<ASTNode>;

template<typename T>
using VectorOf = std::vector<T>;

template<typename A, typename B>
using KeyValue = std::pair<A, B>;

using ElementIndex = std::size_t;
using ObjectInitializer = VectorOf<KeyValue<ElementIndex, IValue*>>;
using ObjectInitializerData = VectorOf<KeyValue<ElementIndex, ASTNode>>;

// contains more than one instruction
class IRuntimeStructureSequence : public IRuntimeStructure
{
	NONCOPYABLE(IRuntimeStructureSequence);
public:
	IRuntimeStructureSequence(InstructionSequence&& insns);
	~IRuntimeStructureSequence();

	[[nodiscard]] IValue* ExecuteBlock(CRuntimeContext* const ctx);
	
	[[nodiscard]] auto NumInstructions() const noexcept {
		return m_oInstructions.size();
	}
	[[nodiscard]] auto& GetInstruction(std::size_t idx) const noexcept {
		return m_oInstructions[idx];
	}
protected:
	InstructionSequence m_oInstructions;
};

enum RuntimeFunctionType
{
	fn_regular,
	fn_built_in,
};

class CRuntimeFunctionBase
{
	NONCOPYABLE(CRuntimeFunctionBase);
public:
	CRuntimeFunctionBase() = default;
	virtual ~CRuntimeFunctionBase() = default;
	[[nodiscard]] constexpr virtual RuntimeFunctionType FunctionType() const noexcept = 0;

	[[maybe_unused]] virtual IValue* ExecuteFunction(CRuntimeContext* const ctx, IValue* _this, VectorOf<IValue*>& args,
		const VariableCaptures& captures) = 0;
};

class CRuntimeFunction final : public CRuntimeFunctionBase, public IRuntimeStructureSequence
{
	friend class CFunction;
	NONCOPYABLE(CRuntimeFunction);

public:
	CRuntimeFunction(ElementIndex moduleIndex, CFunctionBlock& linterFunction,
		VectorOf<CCrossModuleReference>&& args,
		VectorOf<CCrossModuleReference>&& variableIndices);
	~CRuntimeFunction();

	[[nodiscard]] constexpr RuntimeFunctionType FunctionType() const noexcept override { return fn_regular; }

	[[maybe_unused]] inline IValue* ExecuteFunction(CRuntimeContext* const ctx, IValue* _this, 
		VectorOf<IValue*>& args, const VariableCaptures& captures) override {

		return Execute(ctx, _this, args, captures);
	}

	[[nodiscard]] constexpr auto& GetName() const noexcept { return m_sName; }
	[[nodiscard]] constexpr auto& GetModuleIndex() const noexcept { return m_uModuleIndex; }

	[[maybe_unused]] IValue* Execute(CRuntimeContext* const ctx, IValue* _this, VectorOf<IValue*>& args,
		const VariableCaptures& captures) override;
protected:

	[[nodiscard]] constexpr EStructureType Type() const noexcept override { return st_function; };

	ElementIndex m_uModuleIndex{ 0u };

	std::string m_sName;
	std::size_t m_uNumParameters{ 0u };
	std::size_t m_uNumVariables{ 0u };
	VectorOf<CCrossModuleReference> m_oArgumentIndices;
	VectorOf<CCrossModuleReference> m_oVariableIndices;
};

class CBuiltInRuntimeFunction : public CRuntimeFunctionBase
{
	NONCOPYABLE(CBuiltInRuntimeFunction);

public:
	CBuiltInRuntimeFunction(Method_t method, std::size_t numArgs );
	~CBuiltInRuntimeFunction();

	[[nodiscard]] constexpr RuntimeFunctionType FunctionType() const noexcept override { return fn_built_in; }

	[[maybe_unused]] IValue* ExecuteFunction(CRuntimeContext* const ctx, IValue* _this, VectorOf<IValue*>& args,
		const VariableCaptures& captures) override;

	Method_t m_pMethod;
	std::size_t m_uNumArguments{};
};

class CRuntimeExpression final : public IRuntimeStructure
{
	NONCOPYABLE(CRuntimeExpression);
	friend class CRuntimeReturnStatement;
	friend class CRuntimeThrowStatement;

public:
	CRuntimeExpression(ASTNode&& ast);
	~CRuntimeExpression();

	[[maybe_unused]] IValue* Execute(CRuntimeContext* const ctx) override;
	[[nodiscard]] IValue* Evaluate(CRuntimeContext* const ctx);
	[[nodiscard]] inline bool HasAST() { return !!m_pAST.get(); }
protected:
	[[nodiscard]] constexpr EStructureType Type() const noexcept override { return st_expression;};

private:
	[[nodiscard]] static IValue* Evaluate(CRuntimeContext* const ctx, const AbstractSyntaxTree* node);
	[[nodiscard]] static IValue* EvaluateLeaf(CRuntimeContext* const ctx, const AbstractSyntaxTree* node);
	[[nodiscard]] static IValue* EvaluatePostfix(CRuntimeContext* const ctx, const PostfixASTNode* node);
	[[nodiscard]] static IValue* EvaluateUnary(CRuntimeContext* const ctx, const UnaryASTNode* node);

	[[nodiscard]] static IValue* EvaluateSequence(CRuntimeContext* const ctx, const AbstractSyntaxTree* node);
	[[nodiscard]] static IValue* EvaluateTernary(CRuntimeContext* const ctx, const TernaryASTNode* node);
	[[nodiscard]] static IValue* EvaluateFmtString(CRuntimeContext* const ctx, const FmtStringASTNode* node);

	[[nodiscard]] static IValue* EvaluateMemberAccess(IValue* operand, const MemberAccessASTNode* node);
	[[nodiscard]] static IValue* EvaluateSubscript(CRuntimeContext* const ctx, IValue* operand, const SubscriptASTNode* node);
	[[nodiscard]] static IValue* EvaluateFunctionCall(CRuntimeContext* const ctx, IValue* operand, const FunctionCallASTNode* node);

	[[nodiscard]] static VectorOf<IValue*> EvaluateList(CRuntimeContext* const ctx, const ExpressionList& list);
	[[nodiscard]] static ObjectInitializer EvaluateObject(CRuntimeContext* const ctx, const ObjectInitializerData& obj);
	ASTNode m_pAST;

};

class CRuntimeConditionalStatement final : public IRuntimeStructureSequence
{
	NONCOPYABLE(CRuntimeConditionalStatement);
	friend class CElseStatementLinter; // so that it can add the chaining
public:
	CRuntimeConditionalStatement(ASTNode&& condition, InstructionSequence&& insns);
	~CRuntimeConditionalStatement();

	[[maybe_unused]] IValue* Execute(CRuntimeContext* const ctx) override;

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
		ASTNode&& init,
		ASTNode&& cond,
		ASTNode&& endExpr, InstructionSequence&& insns);
	~CRuntimeForStatement();

	[[maybe_unused]] IValue* Execute(CRuntimeContext* const ctx) override;

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
	CRuntimeWhileStatement(ASTNode&& condition, InstructionSequence&& insns);
	~CRuntimeWhileStatement();

	[[maybe_unused]] IValue* Execute(CRuntimeContext* const ctx) override;

protected:
	[[nodiscard]] constexpr EStructureType Type() const noexcept override { return st_while; };

private:
	std::unique_ptr<CRuntimeExpression> m_pCondition;
};

class CRuntimeRepeatStatement final : public IRuntimeStructureSequence
{
	NONCOPYABLE(CRuntimeRepeatStatement);
public:
	CRuntimeRepeatStatement(ASTNode&& condition, InstructionSequence&& insns);
	~CRuntimeRepeatStatement();

	[[maybe_unused]] IValue* Execute(CRuntimeContext* const ctx) override;

protected:
	[[nodiscard]] constexpr EStructureType Type() const noexcept override { return st_while; };

private:
	std::unique_ptr<CRuntimeExpression> m_pCondition;
};

class CRuntimeReturnStatement final : public IRuntimeStructure
{
	NONCOPYABLE(CRuntimeReturnStatement);
public:
	CRuntimeReturnStatement(ASTNode&& condition);
	~CRuntimeReturnStatement();

	[[maybe_unused]] IValue* Execute(CRuntimeContext* const ctx) override;

protected:
	[[nodiscard]] constexpr EStructureType Type() const noexcept override { return st_return; };

private:
	ASTNode m_pAST;
};

class CRuntimeTryCatchStatement final : public IRuntimeStructure
{
	NONCOPYABLE(CRuntimeTryCatchStatement);
public:
	CRuntimeTryCatchStatement(const CCrossModuleReference& catchVariable, InstructionSequence&& tryBlock, InstructionSequence&& catchBlock);
	~CRuntimeTryCatchStatement();

	[[maybe_unused]] IValue* Execute(CRuntimeContext* const ctx) override;

protected:
	[[nodiscard]] constexpr EStructureType Type() const noexcept override { return st_try_catch; };
private:
	[[maybe_unused]] IValue* ExecuteCatchBlock(CRuntimeContext* const ctx);

	CCrossModuleReference m_uCatchVariable;
	InstructionSequence m_oTryInstructions;
	InstructionSequence m_oCatchInstructions;
};

class CRuntimeThrowStatement final : public IRuntimeStructure
{
	NONCOPYABLE(CRuntimeThrowStatement);
public:
	CRuntimeThrowStatement(ASTNode&& condition);
	~CRuntimeThrowStatement();

	[[maybe_unused]] IValue* Execute(CRuntimeContext* const ctx) override;

protected:
	[[nodiscard]] constexpr EStructureType Type() const noexcept override { return st_throw; };

private:
	ASTNode m_pAST;
};

//continue and break
class CRuntimeLoopControlStatement final : public IRuntimeStructure {
	NONCOPYABLE(CRuntimeLoopControlStatement);
public:
	CRuntimeLoopControlStatement(EExecutionControl c) : m_eCtrl(c){}
	
	[[maybe_unused]] IValue* Execute(CRuntimeContext* const ctx) override;

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