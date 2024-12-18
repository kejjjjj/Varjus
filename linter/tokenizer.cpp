#include "tokenizer.hpp"
#include "punctuation.hpp"
#include "globalEnums.hpp"
#include "fs/fs_io.hpp"
#include "error.hpp"

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
std::vector<CToken*> CBufferTokenizer::GetTokens()
{
	assert(!m_oTokens.empty());

	std::vector<CToken*> tokens;

	for (auto&& t : m_oTokens)
		tokens.push_back(t.get());

	return tokens;
}
std::unique_ptr<CToken> CBufferTokenizer::ReadToken()
{
	m_oLastScriptPos = m_oScriptPos;

	CToken token;

	if (EndOfBuffer())
		return nullptr;

	while (true) {

		if (!ReadWhiteSpace())
			return nullptr;

		if (IsToken("//")) {
			if (!ReadSingleLineComment())
				return nullptr;
		} else if (IsToken("/*")) {
			if (!ReadMultiLineComment())
				return nullptr;
		} else {
			break;
		}
	}

	if (!ReadWhiteSpace())
		return nullptr;

	token.m_oSourcePosition = m_oParserPosition;

	if (IsDigit(*m_oScriptPos) || *m_oScriptPos == '.' && IsDigit(*(std::next(m_oScriptPos)))) {
		if (!ReadNumber(token)) {
			return nullptr;
		}
	} else if (IsAlpha(*m_oScriptPos) || *m_oScriptPos == '_') {
		if (!ReadName(token)) {
			return nullptr;
		}
	} else if (*m_oScriptPos == '\"') {
		if (!ReadString(token)) {
			return nullptr;
		}
	} else {

		auto&& punc = ReadPunctuation();

		if (punc)
			return punc;

		throw std::exception("a token without a definition");
	}

	return std::make_unique<CToken>(token);
}

bool CBufferTokenizer::IsToken(const std::string& t) noexcept
{
	if ((std::size_t)std::distance(m_oScriptPos, m_oScriptEnd) < t.length())
		return failure;

	const auto end = m_oScriptPos + (std::ptrdiff_t)t.length();
	const auto punctuation = std::string_view(m_oScriptPos, end);

	return punctuation == t;
}

Success CBufferTokenizer::ReadWhiteSpace() noexcept
{
	if (EndOfBuffer())
		return failure;

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
Success CBufferTokenizer::ReadSingleLineComment() noexcept
{
	auto& [line, column] = m_oParserPosition;

	if (EndOfBuffer())
		return failure;

	while (*m_oScriptPos != '\n') {

		if (*m_oScriptPos == '\t')
			column += size_t(4);
		else
			column++;
		
		m_oScriptPos++;

		if (EndOfBuffer())
			break;
	}

	return success;


}

Success CBufferTokenizer::ReadMultiLineComment()
{
	auto& [line, column] = m_oParserPosition;

	if (EndOfBuffer())
		return failure;

	while (!IsToken("*/")) {

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
			throw std::exception("expected to find */ before EOF");
	}

	m_oScriptPos += 2; // */

	return success;
}
Success CBufferTokenizer::ReadNumber(CToken& token) noexcept
{
	auto& [_, column] = m_oParserPosition;

	if (*m_oScriptPos == '.') { //assumes that there is a number next

		//if the character after the dot is not a number, then stop
		if (std::next(m_oScriptPos) == m_oScriptEnd || !IsDigit(*(std::next(m_oScriptPos) ))) {
			return failure;
		}

		token.m_sSource.push_back(*m_oScriptPos++);
		token.m_eTokenType = TokenType::tt_double;

		//parse the integer literal after the .
		if (!ReadInteger(token))
			return failure;

	}

	else if (IsDigit(*m_oScriptPos)) {
		token.m_eTokenType = TokenType::tt_int;

		if (!ReadInteger(token))
			return failure;

		if (EndOfBuffer())
			return success;

		//floating point decimal
		if (*m_oScriptPos == '.') {
			token.m_sSource.push_back(*m_oScriptPos++);
			token.m_eTokenType = TokenType::tt_double;

			//parse the integer literal after the .
			if (!ReadInteger(token))
				return failure;

		}

		//todo -> suffixes
	}

	assert(token.m_sSource.length());

	column += token.m_sSource.length();
	return success;

}
Success CBufferTokenizer::ReadInteger(CToken& token) noexcept
{
	if (EndOfBuffer())
		return failure;

	if (IsDigit(*m_oScriptPos)) {
		token.m_sSource.push_back(*m_oScriptPos++);

		if (EndOfBuffer())
			return success;

		while (IsDigit(*m_oScriptPos)) {
			token.m_sSource.push_back(*m_oScriptPos++);

			if (EndOfBuffer())
				return success;

		}
	}

	return success;
}
Success CBufferTokenizer::ReadString(CToken& token)
{
	auto& [line, column] = m_oParserPosition;

	token.m_eTokenType = tt_string;
	++m_oScriptPos;

	do {
		if (EndOfBuffer())
			return failure;

		if (*m_oScriptPos == '\"')
			break;

		if (*m_oScriptPos == '\n') {
			line++;
			column = 1ull;
		} else {
			column += (*m_oScriptPos == '\t' ? 4 : 1);
		}

		if(*m_oScriptPos == '\\')
			token.m_sSource.push_back(ReadEscapeCharacter());
		else
			token.m_sSource.push_back(*m_oScriptPos);

		m_oScriptPos++;

		if (EndOfBuffer())
			return failure;

	} while (*m_oScriptPos != '\"');

	m_oScriptPos++;  //skip "
	column++;

	return success;
}
std::int8_t CBufferTokenizer::ReadEscapeCharacter()
{
	auto& [line, column] = m_oParserPosition;

	m_oScriptPos++;  //skip '\\'
	column++;

	if (EndOfBuffer()) {
		CLinterErrors::PushError("unexpected end of file");
		return 0;
	}

	std::int8_t c = *m_oScriptPos;

	switch (c) {
	case '\\': c = '\\'; break;
	case 'n': c = '\n'; break;
	case 'r': c = '\r'; break;
	case 't': c = '\t'; break;
	case 'v': c = '\v'; break;
	case 'b': c = '\b'; break;
	case 'f': c = '\f'; break;
	case 'a': c = '\a'; break;
	case '\'': c = '\''; break;
	case '\"': c = '\"'; break;
	case '\?': c = '\?'; break;
	default: 
		CLinterErrors::PushError("unexpected escape character", m_oParserPosition);
		return 0;
	}

	return c;

}
const std::unordered_map<std::string_view, TokenType> reservedKeywords = {
	{"undefined", TokenType::tt_undefined},
	{"false", TokenType::tt_false},
	{"true", TokenType::tt_true},
	{"let", TokenType::tt_let},
	{"fn", TokenType::tt_fn},
	{"if", TokenType::tt_if},
	{"else", TokenType::tt_else},
	{"for", TokenType::tt_for},
	{"while", TokenType::tt_while},
	{"return", TokenType::tt_return},
	{"break", TokenType::tt_break},
	{"continue", TokenType::tt_continue},
	{"try", TokenType::tt_try},
	{"catch", TokenType::tt_catch},
	{"throw", TokenType::tt_throw},
	{"typeof", TokenType::tt_typeof},
	{"import", TokenType::tt_import},
	{"from", TokenType::tt_from},
	{"export", TokenType::tt_export}
};

Success CBufferTokenizer::ReadName(CToken& token) noexcept
{
	auto& [_, column] = m_oParserPosition;

	token.m_sSource.push_back(*m_oScriptPos++);
	token.m_eTokenType = TokenType::tt_name;

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
std::unique_ptr<CToken> CBufferTokenizer::ReadPunctuation() noexcept
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



/***********************************************************************
 > 
***********************************************************************/
std::vector<std::unique_ptr<CToken>> CBufferTokenizer::ParseFileFromFilePath(const std::string& filePath)
{
	const auto reader = IOReader(filePath, false);
	auto fileBuf = reader.IO_Read();

	if (!fileBuf) {
		throw CLinterError("couldn't read the file buffer");
	}

	fileBuf->push_back('\n'); // fixes a crash lol

	auto tokenizer = CBufferTokenizer(*fileBuf);

	if (!tokenizer.Tokenize())
		throw CLinterError("the input file didn't have any parsable tokens");

	return std::move(tokenizer.m_oTokens);
}
std::vector<CToken*> CBufferTokenizer::ConvertTokensToReadOnly(UniqueTokenVector& src)
{
	assert(!src.empty());

	std::vector<CToken*> tokens;

	for (auto& t : src)
		tokens.push_back(t.get());

	return tokens;
}
