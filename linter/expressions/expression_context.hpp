#pragma once

#include "linter/punctuation.hpp"


class PairMatcher final
{
public:
	PairMatcher(Punctuation p);
	[[nodiscard]] bool IsClosing(Punctuation p) const noexcept;
private:
	Punctuation m_eClosingPunctuation{ p_error };
};
