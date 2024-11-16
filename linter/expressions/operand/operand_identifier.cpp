#include "operand.hpp"
#include "operand_identifier.hpp"
#include "linter/expressions/identifier.hpp"
#include "linter/expressions/ast.hpp"
#include "linter/functions/stack.hpp"

#include <cassert>
std::unique_ptr<IOperand> CLinterOperand::ParseIdentifier()
{
	auto operand = std::make_unique<CIdentifierLinter>(m_iterPos, m_iterEnd, m_pScope, m_pOwner);

	if (!operand->ParseIdentifier()) {
		return nullptr;
	}

	return std::make_unique<CIdentifierOperand>(operand->m_pIdentifier);
}

CLinterVariable* CIdentifierOperand::GetVariable(){
	return dynamic_cast<CLinterVariable*>(m_pIdentifier);
}
CLinterFunction* CIdentifierOperand::GetFunction() {
	return dynamic_cast<CLinterFunction*>(m_pIdentifier);
}

[[nodiscard]] UniqueAST CIdentifierOperand::ToAST()
{
	assert(m_pIdentifier);

	if(m_pIdentifier->Type() == mi_variable)
		return std::make_unique<VariableASTNode>(GetVariable()->m_uIndex);
	if(m_pIdentifier->Type() == mi_function)
		return std::make_unique<FunctionASTNode>(GetFunction()->m_uIndex);

	assert(false);
	return nullptr;
}
