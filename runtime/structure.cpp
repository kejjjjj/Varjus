#include <algorithm>
#include <ranges>

#include "runtime/structure.hpp"
#include "linter/expressions/ast.hpp"
#include "linter/functions/function.hpp"

IRuntimeStructure::IRuntimeStructure() = default;
IRuntimeStructure::~IRuntimeStructure() = default;

CRuntimeFunction::CRuntimeFunction(CFunctionBlock& linterFunction) :
	m_sName(linterFunction.m_sName),
	m_uNumParameters(linterFunction.m_uNumParameters),
	m_uNumVariables(linterFunction.m_pStack->GetVariableCount()),
	m_oInstructions(std::move(linterFunction.m_oInstructions))
{

}
CRuntimeFunction::~CRuntimeFunction() = default;

CRuntimeExpression::CRuntimeExpression(std::unique_ptr<AbstractSyntaxTree>&& ast) :
	m_pAST(std::move(ast)){}
CRuntimeExpression::~CRuntimeExpression() = default;
