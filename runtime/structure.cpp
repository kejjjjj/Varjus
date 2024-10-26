#include <algorithm>
#include <ranges>

#include "runtime/structure.hpp"
#include "linter/expressions/ast.hpp"
#include "linter/functions/function.hpp"

CRuntimeStructure::CRuntimeStructure() = default;
CRuntimeStructure::~CRuntimeStructure() = default;

CRuntimeFunction::CRuntimeFunction(CFunctionBlock& linterFunction) :
	m_sName(linterFunction.m_sName),
	uNumParameters(linterFunction.m_uNumParameters),
	m_oInstructions(std::move(linterFunction.m_oInstructions))
{

}
CRuntimeFunction::~CRuntimeFunction() = default;

CRuntimeExpression::CRuntimeExpression(std::unique_ptr<AbstractSyntaxTree>&& ast) :
	m_pAST(std::move(ast)){}
CRuntimeExpression::~CRuntimeExpression() = default;
