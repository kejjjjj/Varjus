#include "runtime/structure.hpp"
#include "linter/expressions/ast.hpp"

CCodeStructure::CCodeStructure() = default;
CCodeStructure::~CCodeStructure() = default;

CCodeExpression::CCodeExpression(std::unique_ptr<AbstractSyntaxTree>&& ast) :
	m_pAST(std::move(ast)){}
CCodeExpression::~CCodeExpression() = default;
