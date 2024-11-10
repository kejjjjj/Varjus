#pragma once

#include "identifier.hpp"
#include "expression_context.hpp"

#include <optional>

enum OperatorPriority : char;
enum Punctuation : char;

struct CExpressionList;
class CMemory;
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
	explicit CLinterOperatorParser(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const stack);
	~CLinterOperatorParser();

	[[maybe_unused]] Success ParseOperator(std::optional<PairMatcher>& eoe, CExpressionList* expression);
	[[nodiscard]] auto GetToken() const { return m_pToken; }

	[[nodiscard]] OperatorPriority GetPriority() const noexcept;

private:
	[[maybe_unused]] Success ParseSequence(std::optional<PairMatcher>& m_oEndOfExpression, CExpressionList* expression);

	[[nodiscard]] bool CheckOperator() const;
	[[nodiscard]] bool IsOperator(const CPunctuationToken& token) const noexcept;
	[[nodiscard]] bool EndOfExpression(const std::optional<PairMatcher>& eoe) const noexcept;

	std::weak_ptr<CScope> m_pScope;
	CMemory* const m_pOwner;
};
