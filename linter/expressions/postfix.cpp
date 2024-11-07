#include "postfix.hpp"
#include "linter/token.hpp"
#include "linter/punctuation.hpp"
#include "linter/scopes/scope.hpp"
#include "linter/declarations/stack.hpp"

#include "ast.hpp"
#include "expression.hpp"
#include "globalEnums.hpp"

#include <cassert>
CPostfixLinter::CPostfixLinter(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const stack) 
	: CVectorLinter(pos, end), m_pScope(scope), m_pOwner(stack)
{
	assert(m_iterPos != m_iterEnd);
}
CPostfixLinter::~CPostfixLinter() = default;

#pragma pack(push)
#pragma warning( disable : 4061)
Success CPostfixLinter::ParsePostfix()
{

	while (m_iterPos != m_iterEnd && (*m_iterPos)->IsOperator()) {
		const auto& asPunctuation = dynamic_cast<CPunctuationToken&>(**m_iterPos);

		if (!IsPostfixOperator(asPunctuation))
			return success;

		switch (asPunctuation.m_ePunctuation) {
		case p_bracket_open:
			m_oPostfixes.emplace_back(ParseSubscript());
			break;
		case p_par_open:
			m_oPostfixes.emplace_back(ParseFunctionCall());
			break;
		default:
			assert(false);
		}

		m_oTokens.emplace_back(&asPunctuation);
	}

	return success;
}
#pragma pack(pop)

bool CPostfixLinter::IsPostfixOperator(const CPunctuationToken& token) const noexcept
{
	return token.m_ePriority == op_postfix;
}

std::unique_ptr<CPostfixSubscript> CPostfixLinter::ParseSubscript()
{
	assert((*m_iterPos)->IsOperator(p_bracket_open));

	std::advance(m_iterPos, 1); // skip [
	CLinterExpression expr(m_iterPos, m_iterEnd, m_pScope, m_pOwner);

	if (!expr.Parse(PairMatcher(p_bracket_open)))
		return nullptr;

	return std::make_unique<CPostfixSubscript>(expr.ToMergedAST());
}

std::unique_ptr<CPostfixFunctionCall> CPostfixLinter::ParseFunctionCall()
{

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

[[nodiscard]] VectorOf<std::unique_ptr<IPostfixBase>> CPostfixLinter::Move() noexcept
{
	return std::move(m_oPostfixes);
}

CPostfixSubscript::CPostfixSubscript(std::unique_ptr<AbstractSyntaxTree>&& ast) : m_pAST(std::move(ast)){}
CPostfixSubscript::~CPostfixSubscript() = default;

std::unique_ptr<AbstractSyntaxTree> CPostfixSubscript::ToAST()
{
	return std::make_unique<SubscriptASTNode>(std::move(m_pAST));
}

/***********************************************************************
 > 
***********************************************************************/
CPostfixFunctionCall::CPostfixFunctionCall() = default;
CPostfixFunctionCall::CPostfixFunctionCall(ExpressionList&& args) 
	:  m_pArgs(std::move(args)){}
CPostfixFunctionCall::~CPostfixFunctionCall() = default;

std::unique_ptr<AbstractSyntaxTree> CPostfixFunctionCall::ToAST()
{
	return std::make_unique<FunctionCallASTNode>(std::move(m_pArgs));
}
