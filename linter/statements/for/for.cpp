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

CForStatementLinter::CForStatementLinter(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const stack)
	: CStatementLinter(pos, end, scope, stack) {

	assert(m_iterPos != m_iterEnd);
}
CForStatementLinter::~CForStatementLinter() = default;

Success CForStatementLinter::Parse()
{
	m_pThisScope->MakeLoopScope();

	if (!ParseIdentifier(TokenType::tt_for))
		return failure;

	if (IsEndOfBuffer() || !(*m_iterPos)->IsOperator(p_par_open)) {
		m_pOwner->GetModule()->PushError("expected a \"(\"", GetIteratorSafe()->m_oSourcePosition);
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



Success CForStatementLinter::ParseInitializer() {

	if (IsEndOfBuffer()) {
		m_pOwner->GetModule()->PushError("expected an expression", GetIteratorSafe()->m_oSourcePosition);
		return failure;
	}


	if (CVariableDeclarationLinter::IsDeclaration((*m_iterPos))) {
		CVariableDeclarationLinter linter(m_iterPos, m_iterEnd, m_pThisScope, m_pOwner);

		if (!linter.ParseIdentifier() || IsEndOfBuffer())
			return failure;

		if ((*m_iterPos)->IsOperator(p_colon)) {
			return ParseRangedForLoop();
		}

		if (!linter.ParseInitializer())
			return failure;

		std::get<0>(m_oData).m_pInitializer = linter.MoveInitializer();
		
		std::advance(m_iterPos, 1);

		return success;
	}


	if ((*m_iterPos)->IsOperator(p_semicolon)) {
		std::advance(m_iterPos, 1);
		return success;
	}


	if (std::distance(m_iterPos, m_iterEnd) > 2 && (*m_iterPos)->Type() == tt_name && (*std::next(m_iterPos))->IsOperator(p_colon)) {
		std::advance(m_iterPos, 1);
		return ParseRangedForLoop();
	}

	
	CLinterExpression linter(m_iterPos, m_iterEnd, m_pThisScope, m_pOwner);
	if (!linter.Parse())
		return failure;

	std::get<0>(m_oData).m_pInitializer = linter.ToMergedAST();
	std::advance(m_iterPos, 1);
	return success;
}
Success CForStatementLinter::ParseCondition() {

	if (IsEndOfBuffer()) {
		m_pOwner->GetModule()->PushError("expected an expression", GetIteratorSafe()->m_oSourcePosition);
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
Success CForStatementLinter::ParseEndExpression() {
	if (IsEndOfBuffer()) {
		m_pOwner->GetModule()->PushError("expected an expression", GetIteratorSafe()->m_oSourcePosition);
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

Success CForStatementLinter::ParseRangedForLoop()
{
	std::advance(m_iterPos, -1); //go back to the initializer
	
	CLinterOperand operand(m_iterPos, m_iterEnd, m_pThisScope, m_pOwner);

	const auto oper = operand.ParseIdentifier();

	if (!oper)
		return failure;

	auto ast = m_pOwner->IsHoisting() ? nullptr : oper->ToAST();

	if (!m_pOwner->IsHoisting() && (!ast || !ast->IsVariable())) {
		m_pOwner->GetModule()->PushError("expected an expression", GetIteratorSafe()->m_oSourcePosition);
		return failure;
	}

	std::advance(m_iterPos, 1); //skip : 

	//the iterable
	CLinterExpression lintIterable(m_iterPos, m_iterEnd, m_pThisScope, m_pOwner);
	
	if (!lintIterable.Parse(PairMatcher(p_par_open)))
		return failure;

	m_eType = for_ranged;

	if (m_pOwner->IsHoisting())
		return success;

	
	m_oData = RangedForLoop{ 
		.m_pIterator = std::dynamic_pointer_cast<VariableASTNode>(ast),
		.m_pIterable = lintIterable.ToMergedAST()
	};

	if (std::get<1>(m_oData).m_pIterator && std::get<1>(m_oData).m_pIterator->m_bIsConst) {
		m_pOwner->GetModule()->PushError("assignment to a constant", std::get<1>(m_oData).m_pIterator->GetCodePosition());
		return failure;
	}

	return success;
}

RuntimeBlock CForStatementLinter::ToRuntimeObject() const
{
	if (m_eType == for_ranged) {

		decltype(auto) iterable = const_cast<ASTNode&&>(std::move(std::get<1>(m_oData).m_pIterable));
		decltype(auto) iterator = const_cast<std::shared_ptr<VariableASTNode>&&>(std::move(std::get<1>(m_oData).m_pIterator));

		return std::make_unique<CRuntimeRangedForStatement>(std::move(iterator), std::move(iterable), m_pThisScope->MoveInstructions());
	}


	decltype(auto) init = const_cast<ASTNode&&>(std::move(std::get<0>(m_oData).m_pInitializer));
	decltype(auto) cond = const_cast<ASTNode&&>(std::move(std::get<0>(m_oData).m_pCondition));
	decltype(auto) endExpr = const_cast<ASTNode&&>(std::move(std::get<0>(m_oData).m_pOnEnd));

	return std::make_unique<CRuntimeForStatement>(
		std::move(init),
		std::move(cond),
		std::move(endExpr),
		m_pThisScope->MoveInstructions()
	);
}