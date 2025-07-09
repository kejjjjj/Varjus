#pragma once

#include "varjus_api/internal/globalDefinitions.hpp"
#include "varjus_api/types/internal/references.hpp"
#include "varjus_api/types/internal/object_declarations.hpp"
#include "varjus_api/types/internal/callbacks.hpp"

#include <vector>
#include <memory>

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
	st_ranged_for,
	st_while,
	st_match,
	st_case,
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
	lc_abort,
};
class CRuntimeFunctionBase;
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
class CRuntimeCaseStatement;

struct CFunctionBlock;

namespace Varjus {
	class CProgramRuntime;
	class CRuntimeModule;

	struct CRuntimeContext
	{
		CProgramRuntime* m_pRuntime;
		CRuntimeModule* m_pModule;
		CFunction* m_pFunction;
	};
}

template<typename T>
using VectorOf = std::vector<T>;
using __ArgumentIndex = std::size_t;
using __VariableIndex = std::size_t;
using __VariableCaptures = std::unordered_map<CCrossModuleReference, CVariable*, CCrossModuleReferenceHasher>;

#pragma pack(push)
VARJUS_WARNING_DISABLE(4266)



class IRuntimeStructure
{
	VARJUS_NONCOPYABLE(IRuntimeStructure);
public:
	IRuntimeStructure();
	virtual ~IRuntimeStructure();

	[[nodiscard]] virtual constexpr EStructureType Type() const noexcept = 0;

	[[nodiscard]] virtual IValue* Execute(
		[[maybe_unused]] Varjus::CRuntimeContext* const ctx) { return nullptr; };

	[[nodiscard]] virtual IValue* Execute(
		[[maybe_unused]] Varjus::CRuntimeContext* const ctx,
		[[maybe_unused]] IValue* _this,
		[[maybe_unused]] VectorOf<IValue*>& args,
		[[maybe_unused]] const __VariableCaptures& captures) { return nullptr; };

	[[nodiscard]] static EExecutionControl ToControlStatement(const IValue* rv);

	[[nodiscard]] virtual CRuntimeCaseStatement* ToCaseStatement() { return nullptr; }

protected:
};

using UniqueAST = std::unique_ptr<AbstractSyntaxTree>;
using ASTNode = std::shared_ptr<AbstractSyntaxTree>;

using __InstructionSequence = VectorOf<std::unique_ptr<IRuntimeStructure>>;

using __RuntimeBlock = std::unique_ptr<IRuntimeStructure>;
using __RuntimeFunction = std::unique_ptr<CRuntimeFunctionBase>;

using __FunctionArgument = __RuntimeBlock;
using __FunctionArguments = VectorOf<__FunctionArgument>;
using __ExpressionList = VectorOf<ASTNode>;

template<typename T>
using VectorOf = std::vector<T>;

template<typename A, typename B>
using __KeyValue = std::pair<A, B>;

using __ElementIndex = std::size_t;
using __ObjectInitializer = VectorOf<__KeyValue<__ElementIndex, IValue*>>;
using __ObjectInitializerData = VectorOf<__KeyValue<__ElementIndex, ASTNode>>;

// contains more than one instruction

class IRuntimeStructureSequence : public IRuntimeStructure
{
	VARJUS_NONCOPYABLE(IRuntimeStructureSequence);
public:
	IRuntimeStructureSequence(__InstructionSequence&& insns);
	~IRuntimeStructureSequence();

	[[nodiscard]] IValue* ExecuteBlock(Varjus::CRuntimeContext* const ctx);
	
	[[nodiscard]] auto NumInstructions() const noexcept {
		return m_oInstructions.size();
	}
	[[nodiscard]] auto& GetInstruction(std::size_t idx) const noexcept {
		return m_oInstructions[idx];
	}
protected:
	__InstructionSequence m_oInstructions;
};

enum RuntimeFunctionType
{
	fn_regular,
	fn_built_in_method,
	fn_built_in,
};

class CRuntimeFunctionBase
{
	VARJUS_NONCOPYABLE(CRuntimeFunctionBase);
public:
	CRuntimeFunctionBase(const VarjusString& name, std::size_t numArgs) : m_sName(name), m_uNumArguments(numArgs) {}
	virtual ~CRuntimeFunctionBase() = default;
	[[nodiscard]] constexpr virtual RuntimeFunctionType FunctionType() const noexcept = 0;

	[[maybe_unused]] virtual IValue* ExecuteFunction(Varjus::CRuntimeContext* const ctx, IValue* _this, VectorOf<IValue*>& args,
		const __VariableCaptures& captures) = 0;

	[[nodiscard]] constexpr auto& GetName() const noexcept { return m_sName; }

protected:
	VarjusString m_sName;
	std::size_t m_uNumArguments{ 0u };
};

class CRuntimeFunction final : public CRuntimeFunctionBase, public IRuntimeStructureSequence
{
	friend class CFunction;
	VARJUS_NONCOPYABLE(CRuntimeFunction);

public:
	CRuntimeFunction(__ElementIndex moduleIndex, CFunctionBlock& linterFunction,
		VectorOf<CCrossModuleReference>&& args,
		VectorOf<CCrossModuleReference>&& variableIndices);
	~CRuntimeFunction();

	[[nodiscard]] constexpr RuntimeFunctionType FunctionType() const noexcept override { return fn_regular; }

	[[maybe_unused]] inline IValue* ExecuteFunction(Varjus::CRuntimeContext* const ctx, IValue* _this, 
		VectorOf<IValue*>& args, const __VariableCaptures& captures) override {

		return Execute(ctx, _this, args, captures);
	}

	[[nodiscard]] constexpr auto& GetModuleIndex() const noexcept { return m_uModuleIndex; }

	[[maybe_unused]] IValue* Execute(Varjus::CRuntimeContext* const ctx, IValue* _this, VectorOf<IValue*>& args,
		const __VariableCaptures& captures) override;
protected:

	[[nodiscard]] constexpr EStructureType Type() const noexcept override { return st_function; };

	__ElementIndex m_uModuleIndex{ 0u };
	std::size_t m_uNumVariables{ 0u };
	VectorOf<CCrossModuleReference> m_oArgumentIndices;
	VectorOf<CCrossModuleReference> m_oVariableIndices;
};

class CBuiltInRuntimeMethod : public CRuntimeFunctionBase
{
	VARJUS_NONCOPYABLE(CBuiltInRuntimeMethod);

public:
	CBuiltInRuntimeMethod(Varjus::Method_t method, std::size_t numArgs);
	~CBuiltInRuntimeMethod();

	[[nodiscard]] constexpr RuntimeFunctionType FunctionType() const noexcept override { return fn_built_in_method; }

	[[maybe_unused]] IValue* ExecuteFunction(Varjus::CRuntimeContext* const ctx, IValue* _this, VectorOf<IValue*>& args,
		const __VariableCaptures& captures) override;

	Varjus::Method_t m_pMethod;
};

class CBuiltInRuntimeFunction : public CRuntimeFunctionBase
{
	VARJUS_NONCOPYABLE(CBuiltInRuntimeFunction);

public:
	CBuiltInRuntimeFunction(const VarjusString& name, Varjus::Function_t function, std::size_t numArgs);
	~CBuiltInRuntimeFunction();

	[[nodiscard]] constexpr RuntimeFunctionType FunctionType() const noexcept override { return fn_built_in; }

	[[maybe_unused]] IValue* ExecuteFunction(Varjus::CRuntimeContext* const ctx, IValue* _this, VectorOf<IValue*>& args,
		const __VariableCaptures& captures) override;

	Varjus::Function_t m_pFunction;
};

using RuntimeAST = ASTNode;

class CRuntimeExpression final : public IRuntimeStructure
{
	VARJUS_NONCOPYABLE(CRuntimeExpression);
	friend class CRuntimeReturnStatement;
	friend class CRuntimeThrowStatement;

public:
	CRuntimeExpression(ASTNode&& ast);
	~CRuntimeExpression();

	[[maybe_unused]] IValue* Execute(Varjus::CRuntimeContext* const ctx) override;
	[[nodiscard]] IValue* Evaluate(Varjus::CRuntimeContext* const ctx);
	[[nodiscard]] inline bool HasAST() { return !!m_pAST.get(); }
	[[nodiscard]] inline ASTNode& GetAST() { return m_pAST; }

protected:
	[[nodiscard]] constexpr EStructureType Type() const noexcept override { return st_expression;};

private:
	[[nodiscard]] static IValue* Evaluate(Varjus::CRuntimeContext* const ctx, RuntimeAST& node);
	[[nodiscard]] static IValue* EvaluateLeaf(Varjus::CRuntimeContext* const ctx, RuntimeAST& node);
	[[nodiscard]] static IValue* EvaluatePostfix(Varjus::CRuntimeContext* const ctx, PostfixASTNode* node);
	[[nodiscard]] static IValue* EvaluateUnary(Varjus::CRuntimeContext* const ctx, UnaryASTNode* node);

	[[nodiscard]] static IValue* EvaluateSequence(Varjus::CRuntimeContext* const ctx, RuntimeAST& node);
	[[nodiscard]] static IValue* EvaluateTernary(Varjus::CRuntimeContext* const ctx, TernaryASTNode* node);
	[[nodiscard]] static IValue* EvaluateFmtString(Varjus::CRuntimeContext* const ctx, FmtStringASTNode* node);

	[[nodiscard]] static IValue* EvaluateMemberAccess(Varjus::CRuntimeContext* const ctx, IValue* operand, const MemberAccessASTNode* node);
	[[nodiscard]] static IValue* EvaluateSubscript(Varjus::CRuntimeContext* const ctx, IValue* operand, SubscriptASTNode* node);
	[[nodiscard]] static IValue* EvaluateFunctionCall(Varjus::CRuntimeContext* const ctx, IValue* operand, FunctionCallASTNode* node);

	[[nodiscard]] static VectorOf<IValue*> EvaluateList(Varjus::CRuntimeContext* const ctx, __ExpressionList& list);
	[[nodiscard]] static __ObjectInitializer EvaluateObject(Varjus::CRuntimeContext* const ctx, __ObjectInitializerData& obj);
	ASTNode m_pAST;

};

class CRuntimeConditionalStatement final : public IRuntimeStructureSequence
{
	VARJUS_NONCOPYABLE(CRuntimeConditionalStatement);
	friend class CElseStatementLinter; // so that it can add the chaining
public:
	CRuntimeConditionalStatement(ASTNode&& condition, __InstructionSequence&& insns);
	~CRuntimeConditionalStatement();

	[[maybe_unused]] IValue* Execute(Varjus::CRuntimeContext* const ctx) override;

protected:
	[[nodiscard]] constexpr EStructureType Type() const noexcept override { return st_conditional; };

private:

	CRuntimeConditionalStatement* SeekLastBlock();


	std::unique_ptr<CRuntimeExpression> m_pCondition;
	std::unique_ptr<CRuntimeConditionalStatement> m_pNext; //else (if)
};

class CRuntimeForStatement final : public IRuntimeStructureSequence
{
	VARJUS_NONCOPYABLE(CRuntimeForStatement);
public:
	CRuntimeForStatement(
		ASTNode&& init,
		ASTNode&& cond,
		ASTNode&& endExpr, __InstructionSequence&& insns);
	~CRuntimeForStatement();

	[[maybe_unused]] IValue* Execute(Varjus::CRuntimeContext* const ctx) override;

protected:
	[[nodiscard]] constexpr EStructureType Type() const noexcept override { return st_for; };

private:
	std::unique_ptr<CRuntimeExpression> m_pInitializer;
	std::unique_ptr<CRuntimeExpression> m_pCondition;
	std::unique_ptr<CRuntimeExpression> m_pOnEnd;
};
class VariableASTNode;
class CRuntimeRangedForStatement final : public IRuntimeStructureSequence
{
	VARJUS_NONCOPYABLE(CRuntimeRangedForStatement);
public:
	CRuntimeRangedForStatement(std::shared_ptr<VariableASTNode>&& iterator, ASTNode&& iterable, __InstructionSequence&& insns);
	~CRuntimeRangedForStatement();

	[[maybe_unused]] IValue* Execute(Varjus::CRuntimeContext* const ctx) override;

protected:
	[[nodiscard]] constexpr EStructureType Type() const noexcept override { return st_ranged_for; };

private:
	std::shared_ptr<VariableASTNode> m_pIterator;
	std::unique_ptr<CRuntimeExpression> m_pIterable;
};

class CRuntimeWhileStatement final : public IRuntimeStructureSequence
{
	VARJUS_NONCOPYABLE(CRuntimeWhileStatement);
public:
	CRuntimeWhileStatement(ASTNode&& condition, __InstructionSequence&& insns);
	~CRuntimeWhileStatement();

	[[maybe_unused]] IValue* Execute(Varjus::CRuntimeContext* const ctx) override;

protected:
	[[nodiscard]] constexpr EStructureType Type() const noexcept override { return st_while; };

private:
	std::unique_ptr<CRuntimeExpression> m_pCondition;
};

class CRuntimeCaseStatement final : public IRuntimeStructureSequence
{
	VARJUS_NONCOPYABLE(CRuntimeCaseStatement);
	friend class CRuntimeMatchStatement;
public:
	CRuntimeCaseStatement(ASTNode&& condition, __InstructionSequence&& insns);
	~CRuntimeCaseStatement();

	[[maybe_unused]] IValue* Execute(Varjus::CRuntimeContext* const ctx) override;
	[[nodiscard]] inline bool IsDefaultClause() const noexcept { return !m_pCondition->HasAST(); }
protected:
	[[nodiscard]] constexpr EStructureType Type() const noexcept override { return st_case; };
	[[nodiscard]] CRuntimeCaseStatement* ToCaseStatement() override { return this; }

private:
	std::unique_ptr<CRuntimeExpression> m_pCondition; //no value == default clause
};
class CRuntimeMatchStatement final : public IRuntimeStructureSequence
{
	VARJUS_NONCOPYABLE(CRuntimeMatchStatement);
public:
	CRuntimeMatchStatement(ASTNode&& condition, CRuntimeCaseStatement* const defaultClause, __InstructionSequence&& insns);
	~CRuntimeMatchStatement();

	[[maybe_unused]] IValue* Execute(Varjus::CRuntimeContext* const ctx) override;

protected:
	[[nodiscard]] constexpr EStructureType Type() const noexcept override { return st_match; };

private:
	[[nodiscard]] std::size_t GetCaseIndex(Varjus::CRuntimeContext* const ctx) const noexcept;

	CRuntimeCaseStatement* const m_pDefaultCase;
	std::unique_ptr<CRuntimeExpression> m_pCondition;
};
class CRuntimeRepeatStatement final : public IRuntimeStructureSequence
{
	VARJUS_NONCOPYABLE(CRuntimeRepeatStatement);
public:
	CRuntimeRepeatStatement(ASTNode&& condition, __InstructionSequence&& insns);
	~CRuntimeRepeatStatement();

	[[maybe_unused]] IValue* Execute(Varjus::CRuntimeContext* const ctx) override;

protected:
	[[nodiscard]] constexpr EStructureType Type() const noexcept override { return st_while; };

private:
	std::unique_ptr<CRuntimeExpression> m_pCondition;
};

class CRuntimeReturnStatement final : public IRuntimeStructure
{
	VARJUS_NONCOPYABLE(CRuntimeReturnStatement);
public:
	CRuntimeReturnStatement(ASTNode&& condition);
	~CRuntimeReturnStatement();

	[[maybe_unused]] IValue* Execute(Varjus::CRuntimeContext* const ctx) override;

protected:
	[[nodiscard]] constexpr EStructureType Type() const noexcept override { return st_return; };

private:
	ASTNode m_pAST;
};

class CRuntimeTryCatchStatement final : public IRuntimeStructure
{
	VARJUS_NONCOPYABLE(CRuntimeTryCatchStatement);
public:
	CRuntimeTryCatchStatement(const CCrossModuleReference& catchVariable, __InstructionSequence&& tryBlock, __InstructionSequence&& catchBlock);
	~CRuntimeTryCatchStatement();

	[[maybe_unused]] IValue* Execute(Varjus::CRuntimeContext* const ctx) override;

protected:
	[[nodiscard]] constexpr EStructureType Type() const noexcept override { return st_try_catch; };
private:
	[[maybe_unused]] IValue* ExecuteCatchBlock(Varjus::CRuntimeContext* const ctx);

	CCrossModuleReference m_uCatchVariable;
	__InstructionSequence m_oTryInstructions;
	__InstructionSequence m_oCatchInstructions;
};

class CRuntimeThrowStatement final : public IRuntimeStructure
{
	VARJUS_NONCOPYABLE(CRuntimeThrowStatement);
public:
	CRuntimeThrowStatement(ASTNode&& condition);
	~CRuntimeThrowStatement();

	[[maybe_unused]] IValue* Execute(Varjus::CRuntimeContext* const ctx) override;

protected:
	[[nodiscard]] constexpr EStructureType Type() const noexcept override { return st_throw; };

private:
	ASTNode m_pAST;
};

//continue and break
class CRuntimeLoopControlStatement final : public IRuntimeStructure {
	VARJUS_NONCOPYABLE(CRuntimeLoopControlStatement);
public:
	CRuntimeLoopControlStatement(EExecutionControl c) : m_eCtrl(c){}
	
	[[maybe_unused]] IValue* Execute(Varjus::CRuntimeContext* const ctx) override;

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
	virtual __RuntimeBlock ToRuntimeObject() const = 0;
};
#pragma pack(pop)