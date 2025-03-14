#include "varjus_api/internal/globalEnums.hpp"
#include "context.hpp"
#include "error.hpp"
#include "expressions/operand/operand_fmt_string.hpp"
#include "fs/fs_io.hpp"
#include "punctuation.hpp"
#include "tokenizer.hpp"

#include <cassert>
#include <filesystem>
#include <format>
#include <iostream>
#include <unordered_map>

constexpr bool IsDigit(VarjusChar c) noexcept
{
	return c >= '0' && c <= '9';
}
constexpr bool IsAlpha(VarjusChar c) {

#ifdef UNICODE
	return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c > 127;
#else
	return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
#endif
}

CBufferTokenizer::CBufferTokenizer(CProgramInformation* const program, const STD_STRING_VIEW& buffer) 
	: m_sSource(buffer), m_eSuccess(success), m_pProgram(program) {

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
	//	STD_COUT << t->m_sSource << '\n';

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

		if (IsToken(VSL("//"))) {
			if (!ReadSingleLineComment())
				return nullptr;
		} else if (IsToken(VSL("/*"))) {
			if (!ReadMultiLineComment())
				return nullptr;
		} else {
			break;
		}
	}

	if (!ReadWhiteSpace())
		return nullptr;

	token.m_oSourcePosition = m_oParserPosition;

	if (IsDigit(*m_oScriptPos) || (*m_oScriptPos == '.' && IsDigit(*(std::next(m_oScriptPos))))) {
		if (!ReadNumber(token)) {
			return nullptr;
		}
	} 
	#ifndef UNICODE
	else if (IsAlpha(*m_oScriptPos) || *m_oScriptPos == '_') {
		if (!ReadName(token)) {
			return nullptr;
		}
	}
	#else
	else if (*m_oScriptPos > 128 || IsAlpha(*m_oScriptPos) || *m_oScriptPos == '_') {
		if (!ReadName(token)) {
			return nullptr;
		}
	} 
	#endif
	else if (*m_oScriptPos == '\"' || *m_oScriptPos == '\'') {
		if (!ReadString(token, *m_oScriptPos)) {
			return nullptr;
		}
	} else if (*m_oScriptPos == '`') {

		if (auto fmtString = ReadFormatString()) {
			return fmtString;
		}

		m_pProgram->PushError(VSL("a token without a definition"), m_oParserPosition);
		return nullptr;
	}
	else {

		if (auto punc = ReadPunctuation()) 
			return punc;

		m_pProgram->PushError(VSL("a token without a definition"), m_oParserPosition);
		return nullptr;
	}

	return std::make_unique<CToken>(token);
}

bool CBufferTokenizer::IsToken(const VarjusString& t) noexcept
{
	if ((std::size_t)std::distance(m_oScriptPos, m_oScriptEnd) < t.length())
		return failure;

	const auto end = m_oScriptPos + (std::ptrdiff_t)t.length();
	const auto punctuation = STD_STRING_VIEW(m_oScriptPos, end);

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

	while (!IsToken(VSL("*/"))) {

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

		if (EndOfBuffer()) {
			m_pProgram->PushError(VSL("expected to find */ before EOF"));
			return failure;
		}
	}

	m_oScriptPos += 2; // */

	return success;
}
Success CBufferTokenizer::ReadNumber(CToken& token)
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

		const auto isFloat = *m_oScriptPos == '.';

		if (token.m_sSource[0] == '0' && (*m_oScriptPos == 'x' || *m_oScriptPos == 'X')) {
			m_oScriptPos++; // skip x
			if (!ReadHex(token))
				return failure;

		}
		//floating point decimal
		else if (isFloat) {
			token.m_sSource.push_back(*m_oScriptPos++);
			token.m_eTokenType = TokenType::tt_double;

			//parse the integer literal after the .
			if (!ReadInteger(token))
				return failure;

		}

		//todo -> suffixes
		if (!isFloat) {
			switch (*m_oScriptPos) {
			case 'u':
			case 'U':
				token.m_eTokenType = tt_uint;
				m_oScriptPos++;
				break;
			default:
				break;
			}
		}
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
Success CBufferTokenizer::ReadHex(CToken& token)
{
	auto& [_, column] = m_oParserPosition;

	if (EndOfBuffer())
		return failure;

	VarjusString hexStr;

	while (true) {

		if (EndOfBuffer()) {
			m_pProgram->PushError(VSL("unexpected end of file"));
			return failure;
		}

		auto c = *m_oScriptPos;

		const auto isHex = 
			(c >= '0' && c <= '9') ||
			(c >= 'a' && c <= 'f') ||
			(c >= 'A' && c <= 'F');

		if (!isHex)
			break;

		hexStr.push_back(*m_oScriptPos++);
		
	}


	try {
#ifdef UNICODE
		auto utf8Hex = LocaleConverter::u16string_to_utf8(hexStr);
		auto intValue = std::stoll(utf8Hex, nullptr, 16);
#else
		auto intValue = std::stoll(hexStr, nullptr, 16);
#endif
		token.m_sSource = fmt::to_string(intValue);
	}
	catch ([[maybe_unused]]std::out_of_range& ex) {
		try {

#ifdef UNICODE
			auto utf8Hex = LocaleConverter::u16string_to_utf8(hexStr);
			auto uintValue = std::stoull(utf8Hex, nullptr, 16);
#else
			auto uintValue = std::stoull(hexStr, nullptr, 16);
#endif
			token.m_sSource = fmt::to_string(uintValue);
		}
		catch ([[maybe_unused]] std::out_of_range& ex) {
			m_pProgram->PushError(VSL("constant value is out of range"), m_oParserPosition);
			return failure;
		}
	}
	column += hexStr.length();

	return success;
}
Success CBufferTokenizer::ReadString(CToken& token, VarjusChar quote)
{
	auto& [line, column] = m_oParserPosition;

	token.m_eTokenType = tt_string;
	++m_oScriptPos;

	do {
		if (EndOfBuffer())
			return failure;

		if (*m_oScriptPos == quote)
			break;

		if (*m_oScriptPos == '\n') {
			m_pProgram->PushError(VSL("newline within a string"), m_oParserPosition);
			return failure;

		} else {
			column += (*m_oScriptPos == '\t' ? 4 : 1);
		}

		if(*m_oScriptPos == VSL('\\'))
			token.m_sSource.push_back(ReadEscapeCharacter());
		else
			token.m_sSource.push_back(*m_oScriptPos);

		m_oScriptPos++;

		if (EndOfBuffer())
			return failure;

	} while (*m_oScriptPos != quote);

	m_oScriptPos++;  //skip "
	column++;

	return success;
}

#define FMT_STRING_CHAR '$'
#define FMT_EXPRESSION_CHAR '{'
#define FMT_EXPRESSION_END_CHAR '}'
#define FMT_EXPRESSION_END_CHAR_P p_curlybracket_close

Success CBufferTokenizer::ParseFmtRawText(CFmtStringToken& token)
{
	auto& [line, column] = m_oParserPosition;
	VarjusString rawText;

	while (!BeginningOfFmtString() && *m_oScriptPos != '`') {
		if (*m_oScriptPos == '\\') {
			rawText.push_back(ReadEscapeCharacter());
		}
		else {
			rawText.push_back(*m_oScriptPos);
			if (*m_oScriptPos == '\n') {
				++line;
				column = 1;
			}
			else {
				column += (*m_oScriptPos == '\t' ? 4 : 1);
			}
		}

		if (++m_oScriptPos == m_oScriptEnd) 
			break;
	}

	if (!rawText.empty()) {
		token.InsertFmtToken(std::make_unique<CToken>(rawText, tt_string), CFmtStringToken::FmtStringTokenType::raw);
	}

	return !EndOfBuffer() ? success : failure;
}
Success CBufferTokenizer::ParseFmtExpression(CFmtStringToken& token)
{
	auto& [line, column] = m_oParserPosition;

	if (!BeginningOfFmtString()) 
		return success;

	assert(*m_oScriptPos == FMT_STRING_CHAR);
	std::advance(m_oScriptPos, 2); // Skip ${
	column += 2;

	if (EndOfBuffer()) {
		m_pProgram->PushError(VSL("expected a \"}\""), m_oParserPosition);
		return failure;
	}

	while (*m_oScriptPos != FMT_EXPRESSION_END_CHAR) {
		auto newToken = ReadToken();
		if (!newToken || EndOfBuffer()) {
			m_pProgram->PushError(VSL("unexpected end of buffer"), m_oParserPosition);
			return failure;
		}

		token.InsertFmtToken(std::move(newToken), CFmtStringToken::FmtStringTokenType::placeholder);
	};

	token.InsertFmtToken(ReadPunctuation(), CFmtStringToken::FmtStringTokenType::placeholder);
	return !EndOfBuffer() ? success : failure;
}
bool CBufferTokenizer::BeginningOfFmtString() const
{
	return !EndOfBuffer() && std::distance(m_oScriptPos, m_oScriptEnd) >= 2 &&
		*m_oScriptPos == FMT_STRING_CHAR && *(std::next(m_oScriptPos)) == FMT_EXPRESSION_CHAR;
}

std::unique_ptr<CToken> CBufferTokenizer::ReadFormatString()
{
	auto& [line, column] = m_oParserPosition;

	auto token = std::make_unique<CFmtStringToken>();
	++m_oScriptPos;


	while (!EndOfBuffer() && *m_oScriptPos != '`') {

		if (!ParseFmtRawText(*token))
			break;

		if (*m_oScriptPos == '`')
			break;
		
		if (!ParseFmtExpression(*token))
			break;
	}

	if (!EndOfBuffer()) {
		m_oScriptPos++;  //skip `
		column++;
	}

	return token;
}
VarjusChar CBufferTokenizer::ReadEscapeCharacter()
{
	auto& [line, column] = m_oParserPosition;

	m_oScriptPos++;  //skip '\\'
	column++;

	if (EndOfBuffer()) {
		m_pProgram->PushError(VSL("unexpected end of file"), m_oParserPosition);
		return 0;
	}

	auto c = *m_oScriptPos;

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
	case '`': c = '`'; break;
	default: 
		m_pProgram->PushError(VSL("unexpected escape character"), m_oParserPosition);
		return 0;
	}

	return c;

}
const std::unordered_map<STD_STRING_VIEW, TokenType> reservedKeywords = {
	{VSL("undefined"), TokenType::tt_undefined},
	{VSL("false"), TokenType::tt_false},
	{VSL("true"), TokenType::tt_true},
	{VSL("let"), TokenType::tt_let},
	{VSL("const"), TokenType::tt_const},
	{VSL("fn"), TokenType::tt_fn},
	{VSL("if"), TokenType::tt_if},
	{VSL("else"), TokenType::tt_else},
	{VSL("for"), TokenType::tt_for},
	{VSL("while"), TokenType::tt_while},
	{VSL("repeat"), TokenType::tt_repeat},
	{VSL("return"), TokenType::tt_return},
	{VSL("break"), TokenType::tt_break},
	{VSL("continue"), TokenType::tt_continue},
	{VSL("try"), TokenType::tt_try},
	{VSL("catch"), TokenType::tt_catch},
	{VSL("throw"), TokenType::tt_throw},
	{VSL("typeof"), TokenType::tt_typeof},
	{VSL("tostring"), TokenType::tt_tostring},
	{VSL("import"), TokenType::tt_import},
	{VSL("from"), TokenType::tt_from},
	{VSL("export"), TokenType::tt_export}
};

Success CBufferTokenizer::ReadName(CToken& token) noexcept
{
	auto& [_, column] = m_oParserPosition;

	token.m_sSource.push_back(*m_oScriptPos++);
	token.m_eTokenType = TokenType::tt_name;

	if (EndOfBuffer())
		return success;

	#ifndef UNICODE
	while (std::isalnum(*m_oScriptPos) || *m_oScriptPos == '_') {

		token.m_sSource.push_back(*m_oScriptPos++);

		if (EndOfBuffer())
			break;

	}
	#else
	while (*m_oScriptPos > 127 || std::isalnum(*m_oScriptPos) || *m_oScriptPos == '_') {

		token.m_sSource.push_back(*m_oScriptPos++);

		if (EndOfBuffer())
			break;

	}
	#endif

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

			const auto punctuation = STD_STRING_VIEW(m_oScriptPos, end);

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

#ifdef UNICODE
VarjusString CBufferTokenizer::FixLittleEndianness(const VarjusString& src)
{
	if (src.length() % 2 != 0)
		throw std::runtime_error("corrupted eof byte in file");

	VarjusString fixed;

	for (auto it = src.begin(); it != src.end(); it += 2) {

		if (*std::next(it) == '\0') {
			fixed.push_back(*it);
			continue;
		}
		auto combined = static_cast<VarjusChar>((*it << 8) | static_cast<VarjusChar>(*std::next(it)));
		fixed.push_back(combined);
	}

	return fixed;
}
#endif

/***********************************************************************
 > 
***********************************************************************/
std::vector<std::unique_ptr<CToken>> CBufferTokenizer::ParseFileFromFilePath(CProgramInformation* const program, const VarjusString& filePath, EncodingType encoding)
{
	if (!std::filesystem::exists(filePath)) {
		program->PushError(fmt::format(VSL("the input file \"{}\" doesn't exist"), filePath));
		return {};
	}

	const auto reader = IOReader(filePath, true);
	auto fileBuf = reader.IO_Read();

	if (!fileBuf) {
		program->PushError(VSL("couldn't read the file buffer"));
		return {};
	}

	if (encoding == e_auto) {
		encoding = reader.GetEncoding();

		if (encoding == e_unknown) {
			throw CLinterError(filePath, VSL("the input file must include the byte order mark!\n"), nullptr);
		}

	}
#ifdef UNICODE
	switch (encoding) {
	case e_auto:
		assert(false);
		break;
	case e_utf8:
	case e_utf16le:
	case e_utf16be:
		break;
	case e_unknown:
	default:
		program->PushError(fmt::format(VSL("encoding type not specified for \"{}\""), filePath));
		return {};
	}
#else
	if (encoding != e_utf8) {
		program->PushError(fmt::format(VSL("the utf8 build of Varjus does not support this encoding for the file\n\"{}\""), filePath));
		return {};
	}
#endif

	if (fileBuf->size() >= fileBuf->max_size() - std::size_t(1ull)) {
		program->PushError(VSL("buffer size is too big"));
		return {};
	}

	fileBuf->push_back('\n'); // fixes a crash lol

	auto tokenizer = CBufferTokenizer(program, *fileBuf);

	if (!tokenizer.Tokenize()) {
		program->PushError(VSL("the input file didn't have any parseable tokens"));
		return {};
	}

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
