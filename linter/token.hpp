#pragma once


#include <tuple>
#include <array>
#include <cassert>

#include "varjus_api/internal/globalDefinitions.hpp"


enum TokenType : signed char
{
	tt_error,
	tt_undefined,
	tt_false,
	tt_true,
	tt_int,
	tt_uint,
	tt_double,
	tt_string,
	tt_fmt_string,
	tt_name,
	tt_operator,
	tt_let,
	tt_const,
	tt_fn,
	tt_if,
	tt_else,
	tt_for,
	tt_while,
	tt_match,
	tt_case,
	tt_default,
	tt_repeat,
	tt_return,
	tt_break,
	tt_continue,
	tt_try,
	tt_catch,
	tt_throw,
	tt_typeof,
	tt_tostring,
	tt_import,
	tt_from,
	tt_export,
	tt_unused_count
};

#define IS_IMMEDIATE(v) v >= TokenType::tt_undefined && v <= TokenType::tt_string

constexpr std::array<const VarjusChar*, tt_unused_count> tokenTypeStrings = {{
	VSL("error"),
	VSL("undefined"),
	VSL("false"),
	VSL("true"),
	VSL("int"),
	VSL("uint"),
	VSL("double"),
	VSL("string"),
	VSL("fmt_string"),
	VSL("name"),
	VSL("operator"),
	VSL("let"),
	VSL("const"),
	VSL("fn"),
	VSL("if"),
	VSL("else"),
	VSL("for"),
	VSL("while"),
	VSL("match"),
	VSL("case"),
	VSL("default"),
	VSL("repeat"),
	VSL("return"),
	VSL("break"),
	VSL("continue"),
	VSL("try"),
	VSL("catch"),
	VSL("throw"),
	VSL("typeof"),
	VSL("tostring"),
	VSL("import"),
	VSL("from"),
	VSL("export")
}};

static_assert(tokenTypeStrings.size() == tt_unused_count);

enum Punctuation : char;
enum OperatorPriority : char;

struct CPunctuation;

class CToken
{
	friend class CBufferTokenizer;

public:

	//expects a parsed string
	CToken() = default;
	CToken(const STD_STRING_VIEW& token, TokenType tt) : m_eTokenType(tt), m_sSource(token){
		assert(token.data() && token.size());
		assert(m_eTokenType != tt_error);

	} 
	virtual ~CToken() = default;

	std::tuple<size_t, size_t> m_oSourcePosition{1, 1};

	[[nodiscard]] constexpr auto Type() const noexcept { return m_eTokenType; }
	[[nodiscard]] virtual bool IsOperator() const noexcept { return false; }
	[[nodiscard]] virtual bool IsOperator([[maybe_unused]]Punctuation p) const noexcept { return false; }

	[[nodiscard]] constexpr auto& Source() const noexcept { return m_sSource; }

private:
	TokenType m_eTokenType{ tt_error };
protected:
	VarjusString m_sSource;
};

class CPunctuationToken final : public CToken
{
public:
	CPunctuationToken(const CPunctuation& p);
	~CPunctuationToken();

	[[nodiscard]] bool IsOperator() const noexcept override { return true; }
	[[nodiscard]] bool IsOperator(Punctuation p) const noexcept override { return m_ePunctuation == p; }

	Punctuation m_ePunctuation{};
	OperatorPriority m_ePriority{};
};