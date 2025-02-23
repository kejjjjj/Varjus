#include "function.hpp"

#include <cassert>

#include "linter/error.hpp"
#include "linter/token.hpp"
#include "linter/punctuation.hpp"
#include "linter/scopes/scope.hpp"
#include "linter/linter.hpp"
#include "linter/functions/stack.hpp"
#include "linter/context.hpp"
#include "linter/modules/module.hpp"
#include "linter/declarations/members.hpp"
#include "api/internal/globalEnums.hpp"
#include "api/internal/structure.hpp"

#ifdef OPTIMIZATIONS
#include "linter/optimizations/optimizations.hpp"
#endif

#include <format>
#include <algorithm>

CFunctionLinter::CFunctionLinter(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const stack)
	: CLinterSingle(pos, end), m_pScope(scope), m_pOwner(stack),
	m_pThisStack(std::make_unique<CStack>(m_pOwner->GetGlobalMemory(), m_pOwner->m_pModule))
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
CFunctionLinter::~CFunctionLinter()
{
#ifdef OPTIMIZATIONS

	if (m_pThisStack && m_pThisStack->m_ConstEvalVariableManager) {
		for (auto& [name, value] : m_pThisStack->m_ConstEvalVariableManager->GetVariableIterator()) {
			assert(value);
			if (value->m_pConstEval)
				value->m_pConstEval->Release();
		}
	}
#endif

	if (m_pThisStack && m_pThisStack->m_VariableManager) {
		auto context = m_pOwner->GetContext();

		for (const auto& [name, value] : m_pThisStack->m_VariableManager->GetVariableIterator()) {
			if (context->m_oAllVariables.Contains(name)) {
				context->m_oAllVariables.Erase(name);
			}
		}
	}

}
Success CFunctionLinter::Parse()
{
	if (!ParseFunctionDeclaration())
		return failure;

	m_pThisStack->m_pFunction = ToFunction();

	if (!ParseFunctionScope())
		return failure;

	assert(m_pOwner && m_pOwner->m_pModule);

	auto count = m_pOwner->m_pModule->GetFunctionCount();
	m_pOwner->m_pModule->AddFunction(ToRuntimeFunction());
	m_pOwner->m_FunctionManager->DeclareFunction(m_oFunctionName, count);
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

	const auto containsFunc = m_pOwner->m_FunctionManager->ContainsFunction(m_oFunctionName);
	const auto containsVar = m_pOwner->m_VariableManager->ContainsVariable(m_oFunctionName);

	if ((containsFunc && m_pOwner->IsHoisting()) || containsVar) {
		CLinterErrors::PushError(std::format("\"{}\" is already defined", m_oFunctionName), GetIteratorSafe()->m_oSourcePosition);
		return failure;
	}


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
	var->m_bParameter = true;
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
		CLinterErrors::PushError("expected a \"{\" or an expression", GetIteratorSafe()->m_oSourcePosition);
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
bool CFunctionLinter::IsIdentifier(const CToken* token) noexcept
{
	return token && token->Type() == tt_name;
}


std::unique_ptr<CFunctionBlock> CFunctionLinter::ToFunction() const
{
	assert(m_pThisStack);

	return std::make_unique<CFunctionBlock>(CFunctionBlock{
		.m_sName=m_oFunctionName,
		.m_uNumParameters =m_oParameters.size(),
		.m_pStack=m_pThisStack.get(),
		.m_oInstructions={}
	});
}
RuntimeBlock CFunctionLinter::ToRuntimeObject() const{
	return ToRuntimeFunction();
}
std::unique_ptr<CRuntimeFunction> CFunctionLinter::ToRuntimeFunction() const
{
	assert(m_pThisStack != nullptr && m_pThisStack->IsStack());

	const auto stack = m_pThisStack->ToStack();

	assert(stack->m_pFunction != nullptr);
	stack->m_pFunction->m_oInstructions = std::move(m_pThisScope->MoveInstructions());

	auto&& ret = std::make_unique<CRuntimeFunction>(
		m_pOwner->GetGlobalMemory()->m_pModule->GetIndex(),
		*stack->m_pFunction, 
		GetParameterIndices(stack),
		GetVariableIndices(stack));

	m_pThisStack->m_pFunction = nullptr;

	return ret;
}
VectorOf<CCrossModuleReference> CFunctionLinter::GetParameterIndices(CStack* stack) const{
	VectorOf<CCrossModuleReference> refs;

	
	for (auto& [name, var] : stack->m_VariableManager->GetVariableIterator()) {

		if (var->m_bParameter)
			refs.push_back(*var);
	}

	std::sort(refs.begin(), refs.end(), [](const CCrossModuleReference& a, const CCrossModuleReference& b) {
		return a.m_uIndex < b.m_uIndex; });

	return refs;
}
VectorOf<CCrossModuleReference> CFunctionLinter::GetVariableIndices(CStack* stack) const
{
	VectorOf<CCrossModuleReference> indices;

	for (auto& [name, var] : stack->m_VariableManager->GetVariableIterator()) {

		//exclude parameters
		if (var->m_bParameter)
			continue;

		//exclude captured values
		//if (var->m_bCaptured)
		//	continue;

		indices.push_back(*var);
	}

	return indices;
}
VectorOf<CCrossModuleReference> CFunctionLinter::GetSharedOwnershipVariables(CStack* stack) const
{
	VectorOf<CCrossModuleReference> refs;

	for (auto& [name, var] : stack->m_VariableManager->GetVariableIterator()) {

		if (var->m_bCaptured)
			refs.push_back(*var);
	}

	return refs;
}
