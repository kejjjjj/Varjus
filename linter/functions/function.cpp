#include "function.hpp"

#include <cassert>

#include "linter/error.hpp"
#include "linter/token.hpp"
#include "linter/punctuation.hpp"
#include "globalEnums.hpp"
#include <iostream>

CFunctionLinter::CFunctionLinter(LinterIterator& pos, LinterIterator& end, CMemoryData* const stack)
	: m_iterPos(pos), m_iterEnd(end), m_pOwner(stack) {

	assert(m_iterPos != m_iterEnd);
}

Success CFunctionLinter::ParseFunctionDeclaration()
{

	if (IsEndOfBuffer() || !IsFn((*m_iterPos))) {
		CLinterErrors::PushError("expected \"fn\"", IsEndOfBuffer() ? (*std::prev(m_iterPos))->m_oSourcePosition : (*m_iterPos)->m_oSourcePosition);
		return failure;
	}

	std::advance(m_iterPos, 1); //skip fn

	if (IsEndOfBuffer() || !IsIdentifier((*m_iterPos))) {
		CLinterErrors::PushError("expected an identifier", IsEndOfBuffer() ? (*std::prev(m_iterPos))->m_oSourcePosition : (*m_iterPos)->m_oSourcePosition);
		return failure;
	}

	m_oFunctionName = (*m_iterPos)->Source();

	std::advance(m_iterPos, 1); //skip identifier
	std::cout << m_oFunctionName << '\n';
	return ParseFunctionParameters();
}
Success CFunctionLinter::ParseFunctionParameters()
{
	if(IsEndOfBuffer() || !(*m_iterPos)->IsOperator(p_par_open))
		CLinterErrors::PushError("expected a \"(\"", IsEndOfBuffer() ? (*std::prev(m_iterPos))->m_oSourcePosition :  (*m_iterPos)->m_oSourcePosition);

	std::advance(m_iterPos, 1); //skip (

	//no parameters?
	if (!IsEndOfBuffer() && (*m_iterPos)->IsOperator(p_par_close))
		return success;

	if (!ParseFunctionParametersRecursively())
		return failure;

	std::advance(m_iterPos, 1);
	return success;
}

Success CFunctionLinter::ParseFunctionParametersRecursively()
{
	if (IsEndOfBuffer() || !IsIdentifier((*m_iterPos))) {
		CLinterErrors::PushError("expected an identifier", IsEndOfBuffer() ? (*std::prev(m_iterPos))->m_oSourcePosition : (*m_iterPos)->m_oSourcePosition);
		return failure;
	}
	
	m_oParameters.push_back((*m_iterPos)->Source());
	std::cout << m_oParameters.back() << '\n';

	std::advance(m_iterPos, 1); //skip identifier

	if (IsEndOfBuffer()) {
		CLinterErrors::PushError("expected \",\" or \")\"", IsEndOfBuffer() ? (*std::prev(m_iterPos))->m_oSourcePosition : (*m_iterPos)->m_oSourcePosition);
		return failure;
	}

	if ((*m_iterPos)->IsOperator(p_comma)) {
		std::advance(m_iterPos, 1);
		return ParseFunctionParametersRecursively();
	}else if ((*m_iterPos)->IsOperator(p_par_close)) {
		return success;
	}

	CLinterErrors::PushError("expected \",\" or \")\"", IsEndOfBuffer() ? (*std::prev(m_iterPos))->m_oSourcePosition : (*m_iterPos)->m_oSourcePosition);
	return failure;
}
bool CFunctionLinter::IsFn(const CToken* token) const noexcept
{
	return token && token->Type() == t_fn;
}
bool CFunctionLinter::IsIdentifier(const CToken* token) const noexcept
{
	return token && token->Type() == t_name;
}