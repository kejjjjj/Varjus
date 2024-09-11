#include "token.hpp"
#include "punctuation.hpp"

#include <cassert>

constexpr CToken::CToken(const std::string_view& token, TokenType tt) : m_eTokenType(tt), m_sSource(token)
{
	assert(token.data() && token.size());
	assert(m_eTokenType != t_error);
}
constexpr CToken::~CToken() = default;


CPunctuationToken::CPunctuationToken(const CPunctuation& p) 
	: CToken(p.m_sIdentifier, t_operator), m_ePunctuation(p.m_ePunctuation), m_ePriority(p.m_ePriority)
{

}
CPunctuationToken::~CPunctuationToken() = default;