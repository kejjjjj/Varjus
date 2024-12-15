#pragma once

#include <string>
#include <vector>
#include <memory>

#include "token.hpp"

enum Success : signed char;

using UniqueTokenVector = std::vector<std::unique_ptr<CToken>>;

//throws on failure
class CBufferTokenizer final
{
public:
	CBufferTokenizer(const std::string_view& buffer);
	~CBufferTokenizer();
	[[maybe_unused]] Success Tokenize();

	[[nodiscard]] std::vector<CToken*> GetTokens();

private:

	[[nodiscard]] constexpr bool EndOfBuffer() const noexcept { return m_oScriptPos == m_oScriptEnd; }
	[[nodiscard]] constexpr bool EndOfBuffer(const std::string_view::iterator& pos) const noexcept { return pos == m_oScriptEnd; }

	[[nodiscard]] std::unique_ptr<CToken> ReadToken();

	[[nodiscard]] bool IsToken(const std::string& t) noexcept;

	[[nodiscard]] Success ReadWhiteSpace() noexcept;
	[[nodiscard]] Success ReadSingleLineComment() noexcept;
	[[nodiscard]] Success ReadMultiLineComment();

	[[nodiscard]] Success ReadNumber(CToken& token) noexcept;
	[[nodiscard]] Success ReadInteger(CToken& token) noexcept;
	[[nodiscard]] Success ReadString(CToken& token) noexcept;
	[[nodiscard]] Success ReadName(CToken& token) noexcept;

	[[nodiscard]] std::unique_ptr<CToken> ReadPunctuation() noexcept;



	std::string_view::iterator m_oScriptPos;
	std::string_view::iterator m_oLastScriptPos;
	std::string_view::iterator m_oScriptEnd;

	std::tuple<size_t, size_t> m_oParserPosition;

	UniqueTokenVector m_oTokens;

	const std::string_view& m_sSource;
	Success m_eSuccess;

public:

	static UniqueTokenVector ParseFileFromFilePath(const std::string& filePath);
	static std::vector<CToken*> ConvertTokensToReadOnly(UniqueTokenVector& src);
};

