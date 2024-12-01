#include "operand.hpp"
#include "operand_identifier.hpp"
#include "linter/expressions/identifier.hpp"
#include "linter/expressions/ast.hpp"
#include "linter/functions/stack.hpp"
#include "linter/token.hpp"

#include <cassert>
std::unique_ptr<IOperand> CLinterOperand::ParseIdentifier()
{
	auto& oldIter = m_iterPos;
	auto operand = std::make_unique<CIdentifierLinter>(m_iterPos, m_iterEnd, m_pScope, m_pOwner);

	if (!operand->ParseIdentifier()) {
		return nullptr;
	}

	auto&& ptr = std::make_unique<CIdentifierOperand>(operand->m_pIdentifier);
	ptr->m_oCodePosition = (*oldIter)->m_oSourcePosition;
	return ptr;
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

	if (m_pIdentifier->Type() == mi_variable) 
		return std::make_unique<VariableASTNode>(m_oCodePosition, GetVariable()->m_uIndex, GetVariable()->IsGlobal());
	if(m_pIdentifier->Type() == mi_function)
		return std::make_unique<FunctionASTNode>(m_oCodePosition, GetFunction()->m_uIndex);

	assert(false);
	return nullptr;
}
