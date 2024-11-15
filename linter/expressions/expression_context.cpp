#include "expression_context.hpp"

#include <cassert>

PairMatcher::PairMatcher(Punctuation p) {
	assert(
		p == p_par_open || 
		p == p_bracket_open || 
		p == p_curlybracket_open || 
		p == p_comma ||
		p == p_colon
	);
	if (p == p_par_open)
		m_eClosingPunctuation = p_par_close;
	else if (p == p_bracket_open)
		m_eClosingPunctuation = p_bracket_close;
	else if (p == p_curlybracket_open)
		m_eClosingPunctuation = p_curlybracket_close;
	else if (p == p_comma)
		m_eClosingPunctuation = p_comma;
	else if (p == p_colon)
		m_eClosingPunctuation = p_colon;
}

bool PairMatcher::IsClosing(Punctuation p) const noexcept {
	return p == m_eClosingPunctuation;
}
