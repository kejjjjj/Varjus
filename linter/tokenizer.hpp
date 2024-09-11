#pragma once

#include <string>
#include <vector>
#include <memory>

enum Success : signed char;
class CToken;


//throws on failure
class CBufferTokenizer
{
public:
	CBufferTokenizer(const std::string_view& buffer);
	~CBufferTokenizer();
	[[nodiscard]] Success Tokenize();

	[[nodiscard]] std::vector<CToken*> GetResult();

private:

	[[nodiscard]] constexpr bool EndOfBuffer() const noexcept { return m_oScriptPos == m_oScriptEnd; }
	[[nodiscard]] constexpr bool EndOfBuffer(const std::string_view::iterator& pos) const noexcept { return pos == m_oScriptEnd; }

	[[nodiscard]] constexpr std::unique_ptr<CToken> ReadToken();

	[[nodiscard]] constexpr Success ReadWhiteSpace() noexcept;
	[[nodiscard]] constexpr Success ReadNumber(CToken& token) noexcept;
	[[nodiscard]] constexpr Success ReadInteger(CToken& token) noexcept;
	[[nodiscard]] constexpr Success ReadName(CToken& token) noexcept;

	[[nodiscard]] constexpr std::unique_ptr<CToken> ReadPunctuation() noexcept;



	std::string_view::iterator m_oScriptPos;
	std::string_view::iterator m_oLastScriptPos;
	std::string_view::iterator m_oScriptEnd;

	std::tuple<size_t, size_t> m_oParserPosition;

	std::vector<std::unique_ptr<CToken>> m_oTokens;

	const std::string_view& m_sSource;
	Success m_eSuccess;
};

