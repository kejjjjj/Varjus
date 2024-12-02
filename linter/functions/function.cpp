#include "function.hpp"

#include <cassert>

#include "linter/error.hpp"
#include "linter/token.hpp"
#include "linter/punctuation.hpp"
#include "linter/scopes/scope.hpp"
#include "linter/linter.hpp"
#include "linter/functions/stack.hpp"
#include "linter/context.hpp"
#include "globalEnums.hpp"

#include "runtime/structure.hpp"
#include <iostream>

CFunctionLinter::CFunctionLinter(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const stack)
	: CLinterSingle(pos, end), m_pScope(scope), m_pOwner(stack),
	m_pThisStack(std::make_unique<CStack>(m_pOwner->GetGlobalMemory(), m_pOwner->m_pFile, m_pOwner->m_pContext))
{

	if (const auto s = m_pScope.lock()) {
		m_pThisScope = s->CreateScope();
	}
	else {
		CLinterErrors::PushError("!(const auto s = m_pScope.lock())", (*m_iterPos)->m_oSourcePosition);
	}

	m_pThisStack->m_pLowerRegion = m_pOwner;
	assert(m_iterPos != m_iterEnd);
}

Success CFunctionLinter::Parse()
{
	if (!ParseFunctionDeclaration())
		return failure;

	m_pThisStack->m_pFunction = ToFunction();

	if (!ParseFunctionScope())
		return failure;

	assert(m_pOwner && m_pOwner->m_pFile);

	m_pOwner->m_pFile->AddFunction(ToRuntimeFunction());
	m_pOwner->m_FunctionManager->DeclareFunction(m_oFunctionName);

	return success;
}

Success CFunctionLinter::ParseFunctionDeclaration()
{

	if (IsEndOfBuffer() || !IsFn((*m_iterPos))) {
		CLinterErrors::PushError("expected \"fn\"", GetIteratorSafe()->m_oSourcePosition);
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
		CLinterErrors::PushError("expected an identifier", GetIteratorSafe()->m_oSourcePosition);
		return failure;
	}

	m_oFunctionName = (*m_iterPos)->Source();
	std::advance(m_iterPos, 1); //skip identifier

	if (!ParseFunctionParameters())
		return failure;

	return success;
}
Success CFunctionLinter::ParseFunctionParameters()
{
	if (IsEndOfBuffer() || !(*m_iterPos)->IsOperator(p_par_open)) {
		CLinterErrors::PushError("expected a \"(\"", GetIteratorSafe()->m_oSourcePosition);
		return failure;
	}
	std::advance(m_iterPos, 1); //skip (

	//no parameters?
	if (!IsEndOfBuffer() && (*m_iterPos)->IsOperator(p_par_close)) {
		std::advance(m_iterPos, 1);
		return success;
	}

	if (!ParseFunctionParametersRecursively())
		return failure;

	std::advance(m_iterPos, 1);
	return success;
}

Success CFunctionLinter::ParseFunctionParametersRecursively()
{
	if (IsEndOfBuffer() || !IsIdentifier((*m_iterPos))) {
		CLinterErrors::PushError("expected an identifier", GetIteratorSafe()->m_oSourcePosition);
		return failure;
	}
	
	if (!m_pThisScope->DeclareVariable((*m_iterPos)->Source())) {
		CLinterErrors::PushError("variable " + (*m_iterPos)->Source() + " already declared", (*m_iterPos)->m_oSourcePosition);
		return failure;
	}

	auto var = m_pThisStack->m_VariableManager->DeclareVariable((*m_iterPos)->Source());
	m_pThisStack->AddArgumentVariable(var->m_uIndex);
	m_oParameters.push_back((*m_iterPos)->Source());

	std::advance(m_iterPos, 1); //skip identifier

	if (IsEndOfBuffer()) {
		CLinterErrors::PushError("expected \",\" or \")\"", GetIteratorSafe()->m_oSourcePosition);
		return failure;
	}

	if ((*m_iterPos)->IsOperator(p_comma)) {
		std::advance(m_iterPos, 1);
		return ParseFunctionParametersRecursively();
	}else if ((*m_iterPos)->IsOperator(p_par_close)) {
		return success;
	}

	CLinterErrors::PushError("expected \",\" or \")\"", GetIteratorSafe()->m_oSourcePosition);
	return failure;
}

Success CFunctionLinter::ParseFunctionScope()
{
	if (IsEndOfBuffer() || !(*m_iterPos)->IsOperator(p_curlybracket_open)) {
		CLinterErrors::PushError("expected a \"{\"", GetIteratorSafe()->m_oSourcePosition);
		return failure;
	}

	auto scope = CScopeLinter(m_iterPos, m_iterEnd, m_pThisScope, &*m_pThisStack);
	if (!scope.Parse()) {
		return failure;
	}

	return success;
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
	assert(m_pThisStack);

	return std::make_unique<CFunctionBlock>(CFunctionBlock{
		.m_sName=m_oFunctionName,
		.m_uNumParameters =m_oParameters.size(),
		.m_pStack=m_pThisStack.get()
	});
}
RuntimeBlock CFunctionLinter::ToRuntimeObject() const{
	return ToRuntimeFunction();
}
std::unique_ptr<CRuntimeFunction> CFunctionLinter::ToRuntimeFunction() const
{
	assert(m_pThisStack != nullptr && m_pThisStack->IsStack());

	const auto stack = m_pThisStack->ToStack();
	//std::cout << std::format("declaring: {} with ({}, {})\n", 
	//	stack->m_pFunction->m_sName, stack->m_oArgumentIndices.size(), stack->m_oIndicesWhichRequireSharedOwnership.size());

	assert(stack->m_pFunction != nullptr);
	stack->m_pFunction->m_oInstructions = std::move(m_pThisScope->MoveInstructions());

	auto&& ret = std::make_unique<CRuntimeFunction>(
		*stack->m_pFunction, 
		GetParameterIndices(stack),
		GetVariableIndices(stack));

	m_pThisStack->m_pFunction = nullptr;

	return ret;
}
VectorOf<std::size_t> CFunctionLinter::GetParameterIndices(CStack* stack) const{
	return VectorOf<std::size_t>(stack->m_oArgumentIndices.begin(), stack->m_oArgumentIndices.end());
}
VectorOf<std::size_t> CFunctionLinter::GetVariableIndices(CStack* stack) const
{
	VectorOf<std::size_t> indices;

	for (auto& [name, var] : stack->m_VariableManager->GetVariableIterator()) {

		if (std::ranges::find(stack->m_oArgumentIndices, var.m_uIndex) != stack->m_oArgumentIndices.end())
			continue;

		if (std::ranges::find(stack->m_oIndicesWhichRequireSharedOwnership, var.m_uIndex) 
			!= stack->m_oIndicesWhichRequireSharedOwnership.end())
			continue;

		indices.emplace_back(var.m_uIndex);
	}

	return indices;
}
VectorOf<std::size_t> CFunctionLinter::GetSharedOwnershipVariables(CStack* stack) const
{

	return VectorOf<std::size_t>(
		stack->m_oIndicesWhichRequireSharedOwnership.begin(),
		stack->m_oIndicesWhichRequireSharedOwnership.end()
	);
}
