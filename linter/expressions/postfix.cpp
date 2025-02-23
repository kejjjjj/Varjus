#include "postfix.hpp"
#include "linter/token.hpp"
#include "linter/punctuation.hpp"
#include "linter/scopes/scope.hpp"
#include "linter/functions/stack.hpp"
#include "linter/error.hpp"
#include "linter/context.hpp"

#include "ast.hpp"
#include "expression.hpp"
#include "api/internal/globalEnums.hpp"

#include <cassert>
#include <ranges>
#include <algorithm>

CPostfixLinter::CPostfixLinter(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const stack) 
	: CVectorLinter(pos, end), m_pScope(scope), m_pOwner(stack)
{
	assert(m_iterPos != m_iterEnd);
}
CPostfixLinter::~CPostfixLinter() = default;

#pragma pack(push)
WARNING_DISABLE(4061)
Success CPostfixLinter::ParsePostfix()
{

	while (!IsEndOfBuffer() && (*m_iterPos)->IsOperator()) {
		const auto& asPunctuation = dynamic_cast<CPunctuationToken&>(**m_iterPos);

		if (!IsPostfixOperator(asPunctuation))
			break;

		switch (asPunctuation.m_ePunctuation) {
		case p_period:
			m_oPostfixes.emplace_back(ParseMemberAccess());
			break;
		case p_bracket_open:
			m_oPostfixes.emplace_back(ParseSubscript());
			break;
		case p_par_open:
			m_oPostfixes.emplace_back(ParseFunctionCall());
			break;
		case p_increment:
			m_oPostfixes.emplace_back(ParseIncrement());
			break;
		case p_decrement:
			m_oPostfixes.emplace_back(ParseDecrement());
			break;
		default:
			assert(false);
		}

		if (m_oPostfixes.size())
			m_oPostfixes.back()->m_oCodePosition = asPunctuation.m_oSourcePosition;

	}

	std::reverse(m_oPostfixes.begin(), m_oPostfixes.end());
	return success;
}
#pragma pack(pop)

bool CPostfixLinter::IsPostfixOperator(const CPunctuationToken& token) const noexcept
{
	return 
		token.m_ePriority == op_postfix || 
		token.m_ePunctuation == p_increment ||
		token.m_ePunctuation == p_decrement;
}

std::unique_ptr<IPostfixBase> CPostfixLinter::ParseMemberAccess()
{
	assert((*m_iterPos)->IsOperator(p_period));
	std::advance(m_iterPos, 1); // skip .

	if (IsEndOfBuffer() || (*m_iterPos)->Type() != tt_name) {
		CLinterErrors::PushError("expected a member name", GetIteratorSafe()->m_oSourcePosition);
		return nullptr;
	}

	const auto& string = (*m_iterPos)->Source();

	std::advance(m_iterPos, 1); // skip this identifier

	auto& members = CFileContext::m_oAllMembers;
	return std::make_unique<CPostfixMemberAccess>(members[string]);
}

std::unique_ptr<IPostfixBase> CPostfixLinter::ParseSubscript()
{
	assert((*m_iterPos)->IsOperator(p_bracket_open));

	std::advance(m_iterPos, 1); // skip [
	CLinterExpression expr(m_iterPos, m_iterEnd, m_pScope, m_pOwner);

	if (!expr.Parse(PairMatcher(p_bracket_open)))
		return nullptr;

	return std::make_unique<CPostfixSubscript>(expr.ToMergedAST());
}

std::unique_ptr<IPostfixBase> CPostfixLinter::ParseFunctionCall()
{

	if (m_pOwner == m_pOwner->GetGlobalMemory()) {
		CLinterErrors::PushError("don't call functions in the global scope - use the main function", GetIteratorSafe()->m_oSourcePosition);
		return nullptr;
	}
	assert((*m_iterPos)->IsOperator(p_par_open));
	std::advance(m_iterPos, 1); // skip (
	
	//no args
	if (!IsEndOfBuffer() && (*m_iterPos)->IsOperator(p_par_close)) {
		std::advance(m_iterPos, 1); // skip )
		return std::make_unique<CPostfixFunctionCall>();
	}

	CLinterExpression expr(m_iterPos, m_iterEnd, m_pScope, m_pOwner);
	if (!expr.Parse(PairMatcher(p_par_open)))
		return nullptr;
	return std::make_unique<CPostfixFunctionCall>(expr.ToExpressionList());
}

std::unique_ptr<IPostfixBase> CPostfixLinter::ParseIncrement() {
	assert(!IsEndOfBuffer() && (*m_iterPos)->IsOperator(p_increment));
	std::advance(m_iterPos, 1);
	return std::make_unique<CPostfixIncrement>();
}

std::unique_ptr<IPostfixBase> CPostfixLinter::ParseDecrement() {
	assert(!IsEndOfBuffer() && (*m_iterPos)->IsOperator(p_decrement));
	std::advance(m_iterPos, 1);
	return std::make_unique<CPostfixDecrement>();
}

[[nodiscard]] VectorOf<std::unique_ptr<IPostfixBase>> CPostfixLinter::Move() noexcept{
	return std::move(m_oPostfixes);
}

ASTNode CPostfixMemberAccess::ToAST(){
	return std::make_shared<MemberAccessASTNode>(m_oCodePosition, m_uGlobalMemberIndex);
}
ASTNode CPostfixSubscript::ToAST() {
	return std::make_shared<SubscriptASTNode>(m_oCodePosition, std::move(m_pAST));
}
ASTNode CPostfixFunctionCall::ToAST() {
	return std::make_shared<FunctionCallASTNode>(m_oCodePosition, std::move(m_pArgs));
}
ASTNode CPostfixIncrement::ToAST() {
	return std::make_shared<PostfixIncrementAST>(m_oCodePosition);
}
ASTNode CPostfixDecrement::ToAST() {
	return std::make_shared<PostfixDecrementAST>(m_oCodePosition);
}

CPostfixSubscript::CPostfixSubscript(ASTNode&& ast) : m_pAST(std::move(ast)){}
CPostfixSubscript::~CPostfixSubscript() = default;



/***********************************************************************
 > 
***********************************************************************/
CPostfixFunctionCall::CPostfixFunctionCall() = default;
CPostfixFunctionCall::CPostfixFunctionCall(ExpressionList&& args) 
	:  m_pArgs(std::move(args)){}
CPostfixFunctionCall::~CPostfixFunctionCall() = default;

