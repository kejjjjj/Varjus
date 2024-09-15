#pragma once

#include "definitions.hpp"

class CUnaryLinter : public CVectorLinter<CPunctuationToken>
{
public:
	CUnaryLinter() = delete;
	explicit CUnaryLinter(LinterIterator& pos, LinterIterator& end);
	~CUnaryLinter();

	[[maybe_unused]] Success ParseUnary();
	[[nodiscard]] auto& GetResult() const { return m_oTokens; }

private:

	[[nodiscard]] bool IsUnaryOperator(const CPunctuationToken& token) const noexcept;

};