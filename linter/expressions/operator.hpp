#pragma once

#include "identifier.hpp"

enum OperatorPriority : char;
enum Punctuation : char;

class CLinterOperator final
{
public:

	CLinterOperator() = default;
	CLinterOperator(OperatorPriority priority, const CPunctuationToken* token) :
		m_ePriority(priority), m_pToken(token) {}

	[[nodiscard]] inline OperatorPriority GetPriority() const noexcept { return m_ePriority; }
	[[nodiscard]] Punctuation GetPunctuation() const noexcept;

	[[nodiscard]] inline const CPunctuationToken* GetToken() const noexcept { return m_pToken; }

	[[nodiscard]] std::string ToString() const noexcept;

private:
	OperatorPriority m_ePriority{};
	const CPunctuationToken* m_pToken;
};

class CLinterOperatorParser final : public CLinterSingle<CPunctuationToken>
{
public:
	CLinterOperatorParser() = delete;
	explicit CLinterOperatorParser(LinterIterator& pos, LinterIterator& end);
	~CLinterOperatorParser();

	[[maybe_unused]] Success ParseOperator();
	[[nodiscard]] auto GetResult() const { return m_pToken; }

	[[nodiscard]] OperatorPriority GetPriority() const noexcept;


	[[nodiscard]] std::string ToString() const noexcept;

private:
	[[nodiscard]] bool CheckOperator() const;
	[[nodiscard]] bool IsOperator(const CPunctuationToken& token) const noexcept;
};
