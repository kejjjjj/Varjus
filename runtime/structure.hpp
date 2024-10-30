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
	st_while,
};

class CRuntimeFunction;
class CFunction;
class IValue;

struct CFunctionBlock;

class IRuntimeStructure
{
	NONCOPYABLE(IRuntimeStructure);
public:
	IRuntimeStructure();
	virtual ~IRuntimeStructure();

	[[nodiscard]] virtual constexpr EStructureType Type() const noexcept = 0;
	[[nodiscard]] virtual bool Execute([[maybe_unused]]CFunction* const thisFunction) { return false; };

protected:
};

using InstructionSequence = std::vector<std::unique_ptr<IRuntimeStructure>>;

// contains more than one instruction
class IRuntimeStructureSequence : public IRuntimeStructure
{
	NONCOPYABLE(IRuntimeStructureSequence);
public:
	IRuntimeStructureSequence(InstructionSequence&& insns);
	~IRuntimeStructureSequence();

	[[nodiscard]] bool ExecuteBlock([[maybe_unused]] CFunction* const thisFunction);

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
	[[maybe_unused]] bool Execute(CFunction* const thisFunction) override;
protected:

	[[nodiscard]] constexpr EStructureType Type() const noexcept { return st_function; };

	std::string m_sName;
	std::size_t m_uNumParameters{ 0u };
	std::size_t m_uNumVariables{ 0u };
};


class CRuntimeExpression final : public IRuntimeStructure
{
	NONCOPYABLE(CRuntimeExpression);
public:
	CRuntimeExpression(std::unique_ptr<AbstractSyntaxTree>&& ast);
	~CRuntimeExpression();

	[[maybe_unused]] bool Execute(CFunction* const thisFunction) override;
	[[nodiscard]] IValue* Evaluate(CFunction* const thisFunction);

protected:
	[[nodiscard]] constexpr EStructureType Type() const noexcept { return st_expression;};

private:
	[[nodiscard]] IValue* Evaluate(CFunction* const thisFunction, AbstractSyntaxTree* node);
	[[nodiscard]] IValue* EvaluateLeaf(CFunction* const thisFunction, AbstractSyntaxTree* node);
	std::unique_ptr<AbstractSyntaxTree> m_pAST;

};

class CRuntimeWhileStatement final : public IRuntimeStructureSequence
{
	NONCOPYABLE(CRuntimeWhileStatement);
public:
	CRuntimeWhileStatement(std::unique_ptr<AbstractSyntaxTree>&& condition, InstructionSequence&& insns);
	~CRuntimeWhileStatement();

	[[maybe_unused]] bool Execute(CFunction* const thisFunction) override;

protected:
	[[nodiscard]] constexpr EStructureType Type() const noexcept { return st_while; };

private:
	std::unique_ptr<CRuntimeExpression> m_pCondition;
};

using RuntimeBlock = std::unique_ptr<IRuntimeStructure>;
using RuntimeFunction = std::unique_ptr<CRuntimeFunction>;

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

private:
	std::vector<RuntimeFunction> m_oFunctions;
};
