#include "token.hpp"
#include "punctuation.hpp"

CPunctuationToken::CPunctuationToken(const CPunctuation& p) 
	: CToken(p.m_sIdentifier, tt_operator), m_ePunctuation(p.m_ePunctuation), m_ePriority(p.m_ePriority)
{
}
CPunctuationToken::~CPunctuationToken() = default;