#include "linter.hpp"
#include "token.hpp"
#include "error.hpp"
#include "globalEnums.hpp"

#include "expressions/expression.hpp"
#include "declarations/variable_declarations.hpp"
#include "declarations/stack.hpp"
#include "functions/function.hpp"
#include "scopes/scope.hpp"

#include <iostream>
#include <cassert>

CFileLinter::CFileLinter(LinterIterator& start, LinterIterator& end) : CLinter(start, end)
{
}

Success CFileLinter::LintOperator(LinterIterator& start, LinterIterator& end, const WeakScope& scope, CMemory* const memory)
{
	assert(memory && memory->IsStack());

	// a new scope
	if ((*start)->IsOperator(p_curlybracket_open)) {
		CScopeLinter scopeLinter(start, end, scope, memory);
		scopeLinter.ParseScope();
		return success;
	}

	// otherwise a normal expression
	return LintExpression(start, end, scope, memory);
}
Success CFileLinter::LintExpression(LinterIterator& start, LinterIterator& end, const WeakScope& scope, CMemory* const memory)
{
	assert(memory && memory->IsStack());

	CLinterExpression linter(start, end, scope, memory);
	if (!linter.ParseExpression()) 
		return failure;

	memory->ToStack()->AddFunctionInstruction(linter.ToRuntimeObject());
	return success;
}

Success CFileLinter::LintDeclaration(LinterIterator& start, LinterIterator& end, const WeakScope& scope, CMemory* const memory)
{
	assert(memory && memory->IsStack());

	CVariableDeclarationLinter linter(start, end, scope, memory);
	if (!linter.ParseDeclaration())
		return failure;

	auto&& possibleInstruction = linter.ToRuntimeObject();

	if(possibleInstruction) //nullptr when no initializer
		memory->ToStack()->AddFunctionInstruction(std::move(possibleInstruction));

	return success;
}

Success CFileLinter::LintFunction(LinterIterator& start, LinterIterator& end, const WeakScope& scope, CMemory* const memory)
{
	assert(memory && !memory->IsStack());

	CFunctionLinter linter(start, end, scope, memory);
	if (!linter.ParseFunction())
		return failure;

	return success;

}
Success CFileLinter::LintToken(LinterIterator& m_iterPos, LinterIterator& m_iterEnd, const WeakScope& scope, CMemory* const memory)
{
	if (m_iterPos == m_iterEnd)
		return failure;

	switch ((*m_iterPos)->Type()) {
	case tt_declaration:
		return LintDeclaration(m_iterPos, m_iterEnd, scope, memory);
	case tt_false:
	case tt_true:
	case tt_int:
	case tt_double:
	case tt_string:
	case tt_name:
		return LintExpression(m_iterPos, m_iterEnd, scope, memory);
	case tt_operator:
		return LintOperator(m_iterPos, m_iterEnd, scope, memory);
	case tt_fn:
		return LintFunction(m_iterPos, m_iterEnd, scope, memory);
	case tt_error:
	default:
		CLinterErrors::PushError("Unexpected token", (*m_iterPos)->m_oSourcePosition);
		return failure;
	}

}
Success CFileLinter::ParseFile()
{

	m_pFile = std::make_unique<CFileRuntimeData>();
	CMemory globalMemory(m_pFile.get());
	auto globalScope = std::make_shared<CScope>(&globalMemory);

	while (!IsEndOfBuffer()) {
		if (!LintToken(m_iterPos, m_iterEnd, globalScope, &globalMemory))
			break;

		if (m_iterPos == m_iterEnd)
			break;

		std::advance(m_iterPos, 1);
	}

	return success;
}

CFileRuntimeData* CFileLinter::GetRuntimeInformation() const noexcept
{
	return m_pFile.get();
}


