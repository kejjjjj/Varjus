#pragma once

#include "linter/expressions/definitions.hpp"
#include "linter/punctuation.hpp"

#include "varjus_api/internal/structure.hpp"
#include "varjus_api/types/internal/references.hpp"

#include <variant>
#include <memory>
class CMemory;


class CDestructuringLinter final : public CLinterSingle<CToken>
{
	VARJUS_NONCOPYABLE(CDestructuringLinter);

public:
	CDestructuringLinter() = delete;
	CDestructuringLinter(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, CMemory* const stack, bool isConst);
	~CDestructuringLinter();

	[[nodiscard]] Varjus::Success Parse();
	[[nodiscard]] constexpr EDestructuredType GetDestructuredType() const noexcept { return m_oParseData.m_eType; }

	[[nodiscard]] static bool IsDestructuringToken(const CToken* token) noexcept;

	[[nodiscard]] inline std::unique_ptr<CDestructuredData> ToData() const {
		return std::make_unique<CDestructuredData>(m_oParseData);
	}

private:

	[[nodiscard]] Varjus::Success ParseIdentifiers(Punctuation untilThisCharacter);

	WeakScope m_pScope;
	CMemory* const m_pOwner = 0;

	CDestructuredData m_oParseData;
	bool m_bConst{};
};