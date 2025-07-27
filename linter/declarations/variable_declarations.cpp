#include "variable_declarations.hpp"
#include "linter/functions/stack.hpp"
#include "linter/token.hpp"
#include "linter/error.hpp"
#include "linter/scopes/scope.hpp"
#include "linter/punctuation.hpp"
#include "linter/expressions/expression.hpp"
#include "linter/expressions/ast.hpp"
#include "linter/modules/module.hpp"
#include "destructuring.hpp"

#include <cassert>

using namespace Varjus;

CVariableDeclarationLinter::CVariableDeclarationLinter(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const stack) :
	CLinterSingle(pos, end), m_pScope(scope), m_pOwner(stack)
{
	assert(m_pOwner != nullptr);
}
CVariableDeclarationLinter::~CVariableDeclarationLinter() = default;

Varjus::Success CVariableDeclarationLinter::Parse(){
	return ParseIdentifier() && ParseInitializer() ? success : failure;
}

Varjus::Success CVariableDeclarationLinter::ParseIdentifier()
{
	if (IsEndOfBuffer() || !IsDeclaration(*m_iterPos)) {
		m_pOwner->GetModule()->PushError(VSL("expected \"let\" or \"const\""), GetIteratorSafe()->m_oSourcePosition);
		return failure;
	}

	m_bIsConst = (*m_iterPos)->Type() == tt_const;

	std::advance(m_iterPos, 1);

	if (IsEndOfBuffer()) {
		m_pOwner->GetModule()->PushError(Varjus::fmt::format(VSL("expected variable name, but found \"{}\""), GetIteratorSafe()->Source()),
			GetIteratorSafe()->m_oSourcePosition);
		return failure;
	}

	if (CDestructuringLinter::IsDestructuringToken(*m_iterPos)) {
		m_eType = EInitializationTarget::destructured;
		CDestructuringLinter linter(m_iterPos, m_iterEnd, m_pScope, m_pOwner, m_bIsConst);

		if (!linter.Parse())
			return failure;

		m_oDeclarationData = linter.ToData();
		assert(std::get<1>(m_oDeclarationData));

		return Varjus::success;
	}

	m_eType = EInitializationTarget::singular;

	if (IsEndOfBuffer() || !IsIdentifier(*m_iterPos)) {
		m_pOwner->GetModule()->PushError(Varjus::fmt::format(VSL("expected variable name, but found \"{}\""), GetIteratorSafe()->Source()),
			GetIteratorSafe()->m_oSourcePosition);
		return failure;
	}

	m_oDeclarationData = DeclareVariable(*m_iterPos, m_pScope, m_pOwner);

	if (!std::get<0>(m_oDeclarationData))
		return failure;

	std::advance(m_iterPos, 1); // skip identifier

	//parse the initializer first, then set const
	std::get<0>(m_oDeclarationData)->m_bConst = false;
	return success;
}

Varjus::Success CVariableDeclarationLinter::ParseInitializer()
{

	if (IsEndOfBuffer()) {
		m_pOwner->GetModule()->PushError(VSL("expected \";\""), GetIteratorSafe()->m_oSourcePosition);
		return failure;
	}

	//let var;
	if ((*m_iterPos)->IsOperator(p_semicolon)) {
		std::get<0>(m_oDeclarationData)->m_bConst = m_bIsConst;

		if (m_eType == EInitializationTarget::destructured) {
			m_pOwner->GetModule()->PushError(VSL("destructuring requires an initializer"), (*m_iterPos)->m_oSourcePosition);
			return failure;
		}

		return success;
	}

	//let var = expression;
	if (!(*m_iterPos)->IsOperator(p_assign)) {
		m_pOwner->GetModule()->PushError(VSL("expected \";\" or \"=\""), (*m_iterPos)->m_oSourcePosition);
		return failure;
	}

	//go back to the identifier
	std::advance(m_iterPos, m_eType == EInitializationTarget::destructured ? 1 : -1);

	CLinterExpression linter(m_iterPos, m_iterEnd, m_pScope, m_pOwner);
	if (!linter.Parse())
		return failure;

	m_pInitializerAST = linter.ToMergedAST();
	if(m_eType == EInitializationTarget::singular)
		std::get<0>(m_oDeclarationData)->m_bConst = m_bIsConst;
	return success;
}
CLinterVariable* CVariableDeclarationLinter::DeclareVariable(const CToken* token, const WeakScope& m_pScope, CMemory* const m_pOwner)
{
	if (const auto scope = m_pScope.lock()) {

		const auto containsFunc = m_pOwner->GetGlobalMemory()->m_FunctionManager->ContainsFunction(token->Source());

		if (containsFunc || !scope->DeclareVariable(token->Source())) {
			m_pOwner->GetModule()->PushError(Varjus::fmt::format(VSL("\"{}\" already declared"), token->Source()), token->m_oSourcePosition);
			return nullptr;
		}

		return m_pOwner->m_VariableManager->DeclareVariable(token->Source());

	}
	
	m_pOwner->GetModule()->PushError(VSL("!(const auto scope = currentScope.lock())"), token->m_oSourcePosition);
	return nullptr;
	
}
bool CVariableDeclarationLinter::IsDeclaration(const CToken* token) noexcept{
	return token && (token->Type() == tt_let || token->Type() == tt_const);
}
bool CVariableDeclarationLinter::IsIdentifier(const CToken* token) noexcept{
	return token && token->Type() == tt_name;
}
__RuntimeBlock CVariableDeclarationLinter::ToRuntimeObject() const
{
	decltype(auto) tempAST = const_cast<ASTNode&>(m_pInitializerAST);

	assert(!IsEndOfBuffer());

	if (m_eType == EInitializationTarget::singular) {
		if (!tempAST)
			return nullptr;
		return std::make_unique<CRuntimeExpression>(std::move(tempAST));
	}
	decltype(auto) tempDecl = const_cast<std::unique_ptr<CDestructuredData>&>(std::get<1>(m_oDeclarationData));

	auto init = std::make_unique<CInitializer>();
	init->m_eTargetType = m_eType;
	init->m_bContainsDeclaration = true;
	init->m_pTarget = std::move(tempDecl);
	init->m_pInitializer = std::move(tempAST);

	return std::make_unique<CRuntimeInitialization>(std::move(init));
}
ASTNode&& CVariableDeclarationLinter::MoveInitializer() {
	return std::move(m_pInitializerAST);
}