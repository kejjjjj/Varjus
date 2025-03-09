#pragma once


#include <vector>
#include <memory>

#include "token.hpp"

enum Success : signed char;

using UniqueTokenVector = std::vector<std::unique_ptr<CToken>>;
class CFmtStringToken;

//throws on failure
class CBufferTokenizer final
{
public:
	CBufferTokenizer(class CProgramInformation* const program, const STD_STRING_VIEW& buffer);
	~CBufferTokenizer();
	[[maybe_unused]] Success Tokenize();
	[[nodiscard]] std::vector<CToken*> GetTokens();

private:

	[[nodiscard]] constexpr bool EndOfBuffer() const noexcept { return m_oScriptPos == m_oScriptEnd; }
	[[nodiscard]] constexpr bool EndOfBuffer(const STD_STRING_VIEW::iterator& pos) const noexcept { return pos == m_oScriptEnd; }

	[[nodiscard]] std::unique_ptr<CToken> ReadToken();

	[[nodiscard]] bool IsToken(const VarjusString& t) noexcept;

	[[nodiscard]] Success ReadWhiteSpace() noexcept;
	[[nodiscard]] Success ReadSingleLineComment() noexcept;
	[[nodiscard]] Success ReadMultiLineComment();

	[[nodiscard]] Success ReadNumber(CToken& token);
	[[nodiscard]] Success ReadInteger(CToken& token) noexcept;
	[[nodiscard]] Success ReadHex(CToken& token);

	[[nodiscard]] Success ReadString(CToken& token, VarjusChar quote);
	[[nodiscard]] std::unique_ptr<CToken> ReadFormatString();
	[[nodiscard]] Success ParseFmtRawText(CFmtStringToken& token);
	[[nodiscard]] Success ParseFmtExpression(CFmtStringToken& token);
	[[nodiscard]] bool BeginningOfFmtString() const;

	[[nodiscard]] VarjusChar ReadEscapeCharacter();

	[[nodiscard]] Success ReadName(CToken& token) noexcept;

	[[nodiscard]] std::unique_ptr<CToken> ReadPunctuation() noexcept;



	STD_STRING_VIEW::iterator m_oScriptPos;
	STD_STRING_VIEW::iterator m_oLastScriptPos;
	STD_STRING_VIEW::iterator m_oScriptEnd;

	std::tuple<size_t, size_t> m_oParserPosition;

	UniqueTokenVector m_oTokens;

	const STD_STRING_VIEW& m_sSource;
	Success m_eSuccess;

	CProgramInformation* const m_pProgram{};

public:
#ifdef UNICODE
	static std::wstring FixLittleEndianness(const std::wstring& src);
#endif
	static UniqueTokenVector ParseFileFromFilePath(CProgramInformation* const program, const VarjusString& filePath, EncodingType encoding);
	static std::vector<CToken*> ConvertTokensToReadOnly(UniqueTokenVector& src);
};

