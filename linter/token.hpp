#pragma once

#include <string>
#include <tuple>

enum TokenType : signed char
{
	t_error,
	t_int,
	t_double,
	t_string,
	t_name,
	t_operator,
	t_declaration,
	t_fn
};

enum Punctuation : char;
enum OperatorPriority : char;

struct CPunctuation;

class CToken
{
	friend class CBufferTokenizer;

public:

	//expects a parsed string
	constexpr CToken() = default;
	constexpr CToken(const std::string_view& token, TokenType tt);
	constexpr virtual ~CToken();

	std::tuple<size_t, size_t> m_oSourcePosition{1, 1};

	constexpr auto Type() const noexcept { return m_eTokenType; }
	constexpr virtual bool IsOperator() const noexcept { return false; }
	constexpr virtual bool IsOperator([[maybe_unused]]Punctuation p) const noexcept { return false; }

	constexpr auto& Source() const noexcept { return m_sSource; }

protected:
	TokenType m_eTokenType{ t_error };
	std::string m_sSource;
};

class CPunctuationToken final : public CToken
{
public:
	CPunctuationToken(const CPunctuation& p);
	~CPunctuationToken();

	constexpr bool IsOperator() const noexcept override { return true; }
	constexpr bool IsOperator(Punctuation p) const noexcept override { return m_ePunctuation == p; }

	Punctuation m_ePunctuation{};
	OperatorPriority m_ePriority{};
};
