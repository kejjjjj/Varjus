#include "token.hpp"
#include "tokenizer.hpp"
#include "punctuation.hpp"
#include "globalEnums.hpp"

#include <cassert>
#include <iostream>
#include <unordered_map>

constexpr bool IsDigit(char c) noexcept
{
	return c >= '0' && c <= '9';
}
constexpr bool IsAlpha(char c) {
	return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

CBufferTokenizer::CBufferTokenizer(const std::string_view& buffer) : m_sSource(buffer), m_eSuccess(success) {

	assert(buffer.data() && buffer.size());

	m_oScriptPos = m_sSource.begin();
	m_oLastScriptPos = m_sSource.end();
	m_oScriptEnd = m_sSource.end();

	m_oParserPosition = std::make_tuple(size_t(1), size_t(1));

	assert(m_oScriptPos != m_oScriptEnd);

}
CBufferTokenizer::~CBufferTokenizer() = default;

Success CBufferTokenizer::Tokenize()
{
	while (auto&& token = ReadToken())
		m_oTokens.emplace_back(std::forward<std::unique_ptr<CToken>&&>(token));

	//for (const auto& t : m_oTokens)
	//	std::cout << t->m_sSource << '\n';

	return m_oTokens.size() > 0u ? success : failure;
}
std::vector<CToken*> CBufferTokenizer::GetResult()
{
	assert(!m_oTokens.empty());

	std::vector<CToken*> tokens;

	for (auto&& t : m_oTokens)
		tokens.push_back(t.get());

	return tokens;
}
constexpr std::unique_ptr<CToken> CBufferTokenizer::ReadToken()
{
	m_oLastScriptPos = m_oScriptPos;

	CToken token;

	if (EndOfBuffer())
		return nullptr;

	if (!ReadWhiteSpace())
		return nullptr;

	token.m_oSourcePosition = m_oParserPosition;

	if (IsDigit(*m_oScriptPos) || *m_oScriptPos == '.' && IsDigit(*(std::next(m_oScriptPos)))) {
		if (!ReadNumber(token)) {
			return nullptr;
		}
	}
	else if (IsAlpha(*m_oScriptPos) || *m_oScriptPos == '_') {
		if (!ReadName(token)) {
			return nullptr;
		}
	}
	else {

		auto&& punc = ReadPunctuation();

		if (punc)
			return punc;

		throw std::exception("a token without a definition");
	}

	return std::make_unique<CToken>(token);
}

constexpr Success CBufferTokenizer::ReadWhiteSpace() noexcept
{

	auto& [line, column] = m_oParserPosition;

	while (*m_oScriptPos <= ' ') {

		if (EndOfBuffer())
			return failure;

		if (*m_oScriptPos == '\n') {
			line++;
			column = size_t(1);
		}
		else {
			if (*m_oScriptPos == '\t')
				column += size_t(4);
			else
				column++;
		}

		m_oScriptPos++;

		if (EndOfBuffer())
			return failure;

	}

	return success;
}
constexpr Success CBufferTokenizer::ReadNumber(CToken& token) noexcept
{
	auto& [_, column] = m_oParserPosition;

	if (*m_oScriptPos == '.') { //assumes that there is a number next

		//if the character after the dot is not a number, then stop
		if (std::next(m_oScriptPos) == m_oScriptEnd || !IsDigit(*(std::next(m_oScriptPos) ))) {
			return failure;
		}

		token.m_sSource.push_back(*m_oScriptPos++);
		token.m_eTokenType = TokenType::t_double;

		//parse the integer literal after the .
		if (!ReadInteger(token))
			return failure;

	}

	else if (IsDigit(*m_oScriptPos)) {
		token.m_eTokenType = TokenType::t_int;

		if (!ReadInteger(token))
			return failure;

		if (EndOfBuffer())
			return success;

		//floating point decimal
		if (*m_oScriptPos == '.') {
			token.m_sSource.push_back(*m_oScriptPos++);
			token.m_eTokenType = TokenType::t_double;

			//parse the integer literal after the .
			if (!ReadInteger(token))
				return failure;

		}
		//an integer
		else {
		}

		//todo -> suffixes
	}

	assert(token.m_sSource.length());

	column += token.m_sSource.length();
	return success;

}
constexpr Success CBufferTokenizer::ReadInteger(CToken& token) noexcept
{
	if (EndOfBuffer())
		return failure;

	if (IsDigit(*m_oScriptPos)) {
		token.m_sSource.push_back(*m_oScriptPos++);

		if (EndOfBuffer())
			return success;

		while (isdigit(*m_oScriptPos)) {
			token.m_sSource.push_back(*m_oScriptPos++);

			if (EndOfBuffer())
				return success;

		}
	}

	return success;
}

const std::unordered_map<std::string_view, TokenType> reservedKeywords = {
	{"let", TokenType::t_declaration},
	{"fn", TokenType::t_fn},
};

constexpr Success CBufferTokenizer::ReadName(CToken& token) noexcept
{
	auto& [_, column] = m_oParserPosition;

	token.m_sSource.push_back(*m_oScriptPos++);
	token.m_eTokenType = TokenType::t_name;

	if (EndOfBuffer())
		return success;

	while (std::isalnum(*m_oScriptPos) || *m_oScriptPos == '_') {

		token.m_sSource.push_back(*m_oScriptPos++);

		if (EndOfBuffer())
			break;

	}

	if (reservedKeywords.contains(token.m_sSource)) {
		token.m_eTokenType = reservedKeywords.at(token.m_sSource);
	}

	column += token.m_sSource.length();
	return success;

}
constexpr std::unique_ptr<CToken> CBufferTokenizer::ReadPunctuation() noexcept
{
	auto& [line, column] = m_oParserPosition;

	for (const auto& i : punctuations) {

		if ((size_t)std::distance(m_oScriptPos, m_oScriptEnd) >= i.m_sIdentifier.length()) {
			const auto end = m_oScriptPos + static_cast<ptrdiff_t>(i.m_sIdentifier.length());

			const auto punctuation = std::string_view(m_oScriptPos, end);

			if (punctuation.empty())
				return nullptr;

			if (!punctuation.compare(i.m_sIdentifier)) {

				auto token = std::make_unique<CPunctuationToken>(i);
				token->m_oSourcePosition = std::tie(line, column);

				column += token->m_sSource.length();
				m_oScriptPos += static_cast<ptrdiff_t>(token->m_sSource.length());

				return token;

			}


		}

	}


	return nullptr;
}
