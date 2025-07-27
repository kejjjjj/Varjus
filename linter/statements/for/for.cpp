#include "for.hpp"

#include "linter/token.hpp"
#include "linter/error.hpp"
#include "linter/punctuation.hpp"
#include "linter/expressions/expression.hpp"
#include "linter/expressions/operand/operand.hpp"
#include "linter/expressions/ast.hpp"
#include "linter/scopes/scope.hpp"
#include "linter/declarations/variable_declarations.hpp"
#include "linter/modules/module.hpp"


#include "varjus_api/internal/globalDefinitions.hpp"

#include <cassert>

using namespace Varjus;

CForStatementLinter::CForStatementLinter(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const stack)
	: CStatementLinter(pos, end, scope, stack) {

	assert(m_iterPos != m_iterEnd);
}
CForStatementLinter::~CForStatementLinter() = default;

Varjus::Success CForStatementLinter::Parse()
{
	m_pThisScope->MakeLoopScope();

	if (!ParseIdentifier(TokenType::tt_for))
		return failure;

	if (IsEndOfBuffer() || !(*m_iterPos)->IsOperator(p_par_open)) {
		m_pOwner->GetModule()->PushError(VSL("expected a \"(\""), GetIteratorSafe()->m_oSourcePosition);
		return failure;
	}

	m_oData = StandardForLoop{};
	m_eType = for_standard;

	std::advance(m_iterPos, 1);

	if (!ParseInitializer())
		return failure;

	if (m_eType == for_ranged)
		return ParseScope();

	if (!ParseCondition())
		return failure;

	if (!ParseEndExpression())
		return failure;

	return ParseScope();
}



Varjus::Success CForStatementLinter::ParseInitializer() {

	if (IsEndOfBuffer()) {
		m_pOwner->GetModule()->PushError(VSL("expected an expression"), GetIteratorSafe()->m_oSourcePosition);
		return failure;
	}

	m_oInitializer = std::make_unique<CInitializer>();

	if (CVariableDeclarationLinter::IsDeclaration((*m_iterPos))) {
		CVariableDeclarationLinter linter(m_iterPos, m_iterEnd, m_pThisScope, m_pOwner);
		auto& pos = (*m_iterPos)->m_oSourcePosition;

		if (!linter.ParseIdentifier() || IsEndOfBuffer())
			return failure;

		m_bConst = false; 
		m_oInitializer->m_eTargetType = linter.m_eType;

		if (linter.m_eType == EInitializationTarget::singular) {
			m_oInitializer->m_pTarget = std::make_shared<VariableASTNode>(pos, std::get<0>(linter.m_oDeclarationData));
		} else {
			m_oInitializer->m_pTarget = std::move(std::get<1>(linter.m_oDeclarationData));
		}

		if ((*m_iterPos)->IsOperator(p_colon)) {
			std::advance(m_iterPos, 1); // skip :
			return ParseRangedForLoop();
		}

		if (!linter.ParseInitializer())
			return failure;

		m_oInitializer->m_pInitializer = linter.MoveInitializer();
		std::advance(m_iterPos, 1);
		return success;
	}

	// no initialization ( just ; )
	if ((*m_iterPos)->IsOperator(p_semicolon)) {
		m_oInitializer->m_bContainsDeclaration = false;
		std::advance(m_iterPos, 1);
		return success;
	}
	
	if (std::distance(m_iterPos, m_iterEnd) > 2 && (*m_iterPos)->Type() == tt_name && (*std::next(m_iterPos))->IsOperator(p_colon)) {
		
		if(!m_pThisScope->VariableExists((*m_iterPos)->Source()))
			m_pOwner->GetModule()->PushError(VSL("expected a variable"), GetIteratorSafe()->m_oSourcePosition);

		const auto var = m_pOwner->m_VariableManager->GetVariable((*m_iterPos)->Source());
		assert(var);

		m_bConst = var->m_bConst;
		m_iterIdentifier = m_iterPos;

		m_oInitializer->m_eTargetType = EInitializationTarget::singular;
		m_oInitializer->m_pTarget = std::make_shared<VariableASTNode>((*m_iterPos)->m_oSourcePosition, var);

		std::advance(m_iterPos, 2); // skip the variable and the :
		return ParseRangedForLoop();
	}

	m_oInitializer->m_bContainsDeclaration = false;

	CLinterExpression linter(m_iterPos, m_iterEnd, m_pThisScope, m_pOwner);
	if (!linter.Parse())
		return failure;

	m_oInitializer->m_pInitializer = linter.ToMergedAST();
	std::advance(m_iterPos, 1);
	return success;
}
Varjus::Success CForStatementLinter::ParseCondition() {

	if (IsEndOfBuffer()) {
		m_pOwner->GetModule()->PushError(VSL("expected an expression"), GetIteratorSafe()->m_oSourcePosition);
		return failure;
	}

	if ((*m_iterPos)->IsOperator(p_semicolon)) {
		std::advance(m_iterPos, 1);
		return success;
	}
	CLinterExpression linter(m_iterPos, m_iterEnd, m_pThisScope, m_pOwner);
	if (!linter.Parse())
		return failure;

	std::get<0>(m_oData).m_pCondition = linter.ToMergedAST();
	std::advance(m_iterPos, 1);
	return success;
}
Varjus::Success CForStatementLinter::ParseEndExpression() {
	if (IsEndOfBuffer()) {
		m_pOwner->GetModule()->PushError(VSL("expected an expression"), GetIteratorSafe()->m_oSourcePosition);
		return failure;
	}

	if ((*m_iterPos)->IsOperator(p_par_close)) {
		std::advance(m_iterPos, 1);
		return success;
	}

	CLinterExpression linter(m_iterPos, m_iterEnd, m_pThisScope, m_pOwner);
	if (!linter.Parse(PairMatcher(p_par_open))) // match the )
		return failure;

	std::get<0>(m_oData).m_pOnEnd = linter.ToMergedAST();
	return success;
}

Varjus::Success CForStatementLinter::ParseRangedForLoop()
{	
	LinterIterator oldPos = m_iterPos;
	CLinterExpression operand(m_iterPos, m_iterEnd, m_pThisScope, m_pOwner);

	const auto oper = operand.Parse(PairMatcher(p_par_open));

	if (!oper)
		return failure;

	auto ast = m_pOwner->IsHoisting() ? nullptr : operand.ToMergedAST();

	if (!m_pOwner->IsHoisting() && (!ast)) {
		m_pOwner->GetModule()->PushError(VSL("expected an expression"), GetIteratorSafe()->m_oSourcePosition);
		return failure;
	}

	m_iterPos = oldPos;

	//the iterable
	CLinterExpression lintIterable(m_iterPos, m_iterEnd, m_pThisScope, m_pOwner);
	
	if (!lintIterable.Parse(PairMatcher(p_par_open)))
		return failure;

	m_eType = for_ranged;

	if (m_pOwner->IsHoisting())
		return success;

	m_oInitializer->m_bContainsDeclaration = true;
	m_oInitializer->m_pInitializer = lintIterable.ToMergedAST();

	m_oData = RangedForLoop{};

	return CheckConstness();
}
Varjus::Success CForStatementLinter::CheckConstness()
{
	if (m_bConst) {
		m_pOwner->GetModule()->PushError(VSL("assignment to a constant"), (*m_iterIdentifier)->m_oSourcePosition);
		return failure;
	}

	return success;
}

__RuntimeBlock CForStatementLinter::ToRuntimeObject() const
{
	if (m_eType == for_ranged) {

		decltype(auto) iterator = const_cast<std::unique_ptr<CInitializer>&>(m_oInitializer);
		return std::make_unique<CRuntimeRangedForStatement>(std::move(iterator), m_pThisScope->MoveInstructions());
	}

	decltype(auto) init = const_cast<std::unique_ptr<CInitializer>&>(m_oInitializer);
	decltype(auto) cond = const_cast<ASTNode&>(std::get<0>(m_oData).m_pCondition);
	decltype(auto) endExpr = const_cast<ASTNode&>(std::get<0>(m_oData).m_pOnEnd);

	return std::make_unique<CRuntimeForStatement>(
		std::move(init),
		std::move(cond),
		std::move(endExpr),
		m_pThisScope->MoveInstructions()
	);
}