#include "function.hpp"

#include <cassert>

#include "linter/error.hpp"
#include "linter/token.hpp"
#include "linter/punctuation.hpp"
#include "linter/scopes/scope.hpp"
#include "linter/linter.hpp"
#include "linter/declarations/stack.hpp"
#include "globalEnums.hpp"

#include "runtime/structure.hpp"

CFunctionLinter::CFunctionLinter(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const stack)
	: m_iterPos(pos), m_iterEnd(end), m_pScope(scope), m_pOwner(stack) {

	assert(m_iterPos != m_iterEnd);
}

Success CFunctionLinter::ParseFunction()
{

	if (!ParseFunctionDeclaration())
		return failure;

	if (!ParseFunctionScope())
		return failure;

	assert(m_pOwner && m_pOwner->m_pFile);

	auto asGeneric = ToRuntimeObject();
	
	//base -> derived conversion 
	m_pOwner->m_pFile->AddFunction(std::unique_ptr<CRuntimeFunction>(static_cast<CRuntimeFunction*>(asGeneric.release())));
	return success;
}

Success CFunctionLinter::ParseFunctionDeclaration()
{

	if (IsEndOfBuffer() || !IsFn((*m_iterPos))) {
		CLinterErrors::PushError("expected \"fn\"", IsEndOfBuffer() ? (*std::prev(m_iterPos))->m_oSourcePosition : (*m_iterPos)->m_oSourcePosition);
		return failure;
	}

	if (const auto scope = m_pScope.lock()) {
		if (!scope->IsGlobalScope()) {
			CLinterErrors::PushError("function declarations are only allowed in the global scope", (*m_iterPos)->m_oSourcePosition);
			return failure;
		}
	} else {
		CLinterErrors::PushError("!(const auto scope = m_pScope.lock())", (*m_iterPos)->m_oSourcePosition);
		return failure;
	}

	std::advance(m_iterPos, 1); //skip fn

	if (IsEndOfBuffer() || !IsIdentifier((*m_iterPos))) {
		CLinterErrors::PushError("expected an identifier", IsEndOfBuffer() ? (*std::prev(m_iterPos))->m_oSourcePosition : (*m_iterPos)->m_oSourcePosition);
		return failure;
	}

	m_oFunctionName = (*m_iterPos)->Source();

	std::advance(m_iterPos, 1); //skip identifier

	if (!ParseFunctionParameters())
		return failure;

	return success;
}
Success CFunctionLinter::ParseFunctionScope()
{
	if (IsEndOfBuffer() || !(*m_iterPos)->IsOperator(p_curlybracket_open)) {
		CLinterErrors::PushError("expected a \"{\"", IsEndOfBuffer() ? (*std::prev(m_iterPos))->m_oSourcePosition : (*m_iterPos)->m_oSourcePosition);
		return failure;
	}

	m_pThisStack = std::make_unique<CStack>(ToFunction(), m_pOwner->m_pFile);
	auto scope = CScopeLinter(m_iterPos, m_iterEnd, m_pScope, &*m_pThisStack);
	if (!scope.ParseScope()) {
		return failure;
	}

	return success;
}
Success CFunctionLinter::ParseFunctionParameters()
{
	if (IsEndOfBuffer() || !(*m_iterPos)->IsOperator(p_par_open)) {
		CLinterErrors::PushError("expected a \"(\"", IsEndOfBuffer() ? (*std::prev(m_iterPos))->m_oSourcePosition : (*m_iterPos)->m_oSourcePosition);
		return failure;
	}
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
	
	if (const auto scope = m_pScope.lock()) {
		if (!scope->DeclareVariable((*m_iterPos)->Source())) {
			CLinterErrors::PushError("variable " + (*m_iterPos)->Source() + " already declared", (*m_iterPos)->m_oSourcePosition);
			return failure;
		}

		m_pOwner->DeclareVariable((*m_iterPos)->Source());
	} else {
		CLinterErrors::PushError("!(const auto scope = currentScope.lock())", (*m_iterPos)->m_oSourcePosition);
		return failure;
	}

	m_oParameters.push_back((*m_iterPos)->Source());

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
	return token && token->Type() == tt_fn;
}
bool CFunctionLinter::IsIdentifier(const CToken* token) const noexcept
{
	return token && token->Type() == tt_name;
}


std::unique_ptr<CFunctionBlock> CFunctionLinter::ToFunction() const
{
	assert(m_pOwner && m_pOwner->IsStack());

	return std::make_unique<CFunctionBlock>(CFunctionBlock{
		.m_sName=m_oFunctionName,
		.m_uNumParameters =m_oParameters.size(),
		.m_pStack=m_pOwner->ToStack()
	});
}
RuntimeBlock CFunctionLinter::ToRuntimeObject() const
{
	assert(m_pThisStack != nullptr && m_pThisStack->IsStack());
	
	const auto stack = m_pThisStack->ToStack();
	assert(stack->m_pFunction != nullptr);

	return std::make_unique<CRuntimeFunction>(*stack->m_pFunction);
}
