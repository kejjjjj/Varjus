#include <algorithm>
#include <ranges>

#include "runtime/structure.hpp"
#include "linter/expressions/ast.hpp"
#include "linter/functions/function.hpp"

IRuntimeStructure::IRuntimeStructure() = default;
IRuntimeStructure::~IRuntimeStructure() = default;

IRuntimeStructureSequence::IRuntimeStructureSequence(InstructionSequence&& insns) 
	: m_oInstructions(std::move(insns)) {}
IRuntimeStructureSequence::~IRuntimeStructureSequence() = default;

CRuntimeFunction::CRuntimeFunction(CFunctionBlock& linterFunction) :
	IRuntimeStructureSequence(std::move(linterFunction.m_oInstructions)),
	m_sName(linterFunction.m_sName),
	m_uNumParameters(linterFunction.m_uNumParameters),
	m_uNumVariables(linterFunction.m_pStack->GetVariableCount())
{

}
CRuntimeFunction::~CRuntimeFunction() = default;

CRuntimeExpression::CRuntimeExpression(std::unique_ptr<AbstractSyntaxTree>&& ast) :
	m_pAST(std::move(ast)){}
CRuntimeExpression::~CRuntimeExpression() = default;


CRuntimeWhileStatement::CRuntimeWhileStatement(std::unique_ptr<AbstractSyntaxTree>&& condition, InstructionSequence&& insns) 
	: IRuntimeStructureSequence(std::move(insns)), m_pCondition(std::make_unique<CRuntimeExpression>(std::move(condition))) {}

CRuntimeWhileStatement::~CRuntimeWhileStatement() = default;


bool IRuntimeStructureSequence::ExecuteBlock([[maybe_unused]] CFunction* const thisFunction)
{
	for (auto& insn : m_oInstructions)
		if (insn->Execute(thisFunction))
			return true;

	return false;
}
