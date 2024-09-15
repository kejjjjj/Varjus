#pragma once

#include "identifier.hpp"

enum OperatorPriority : char;

class CLinterOperatorParser : public CLinterSingle<CPunctuationToken>
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
