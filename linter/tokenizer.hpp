#pragma once


#include <vector>
#include <memory>

#include "token.hpp"

enum Success : signed char;

using UniqueTokenVector = std::vector<std::unique_ptr<CToken>>;
class CFmtStringToken;

namespace Varjus {
	class CProgramInformation;
}

//throws on failure
class CBufferTokenizer final
{
public:
	CBufferTokenizer(Varjus::CProgramInformation* const program, const STD_STRING_VIEW& buffer);
	~CBufferTokenizer();
	[[maybe_unused]] Varjus::Success Tokenize();
	[[nodiscard]] std::vector<CToken*> GetTokens();

private:

	[[nodiscard]] constexpr bool EndOfBuffer() const noexcept { return m_oScriptPos == m_oScriptEnd; }
	[[nodiscard]] constexpr bool EndOfBuffer(const STD_STRING_VIEW::iterator& pos) const noexcept { return pos == m_oScriptEnd; }

	[[nodiscard]] std::unique_ptr<CToken> ReadToken();

	[[nodiscard]] bool IsToken(const VarjusString& t) noexcept;

	[[nodiscard]] Varjus::Success ReadWhiteSpace() noexcept;
	[[nodiscard]] Varjus::Success ReadSingleLineComment() noexcept;
	[[nodiscard]] Varjus::Success ReadMultiLineComment();

	[[nodiscard]] Varjus::Success ReadNumber(CToken& token);
	[[nodiscard]] Varjus::Success ReadInteger(CToken& token) noexcept;
	[[nodiscard]] Varjus::Success ReadHex(CToken& token);

	[[nodiscard]] Varjus::Success ReadString(CToken& token, VarjusChar quote);
	[[nodiscard]] std::unique_ptr<CToken> ReadFormatString();
	[[nodiscard]] Varjus::Success ParseFmtRawText(CFmtStringToken& token);
	[[nodiscard]] Varjus::Success ParseFmtExpression(CFmtStringToken& token);
	[[nodiscard]] bool BeginningOfFmtString() const;

	[[nodiscard]] VarjusChar ReadEscapeCharacter();
	[[nodiscard]] VarjusChar ReadHexCharacter();

	[[nodiscard]] Varjus::Success ReadName(CToken& token) noexcept;

	[[nodiscard]] std::unique_ptr<CToken> ReadPunctuation() noexcept;



	STD_STRING_VIEW::iterator m_oScriptPos;
	STD_STRING_VIEW::iterator m_oLastScriptPos;
	STD_STRING_VIEW::iterator m_oScriptEnd;

	std::tuple<size_t, size_t> m_oParserPosition;

	UniqueTokenVector m_oTokens;

	const STD_STRING_VIEW& m_sSource;
	Varjus::Success m_eSuccess;

	Varjus::CProgramInformation* const m_pProgram{};

public:
#ifdef UNICODE
	static VarjusString FixLittleEndianness(const VarjusString& src);
#endif
	static UniqueTokenVector ParseFileFromFilePath(Varjus::CProgramInformation* const program, const VarjusString& filePath, Varjus::EncodingType encoding);
	static std::vector<CToken*> ConvertTokensToReadOnly(UniqueTokenVector& src);
};

