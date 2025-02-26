#pragma once

#ifdef OPTIMIZATIONS



#include <cstddef>
#include <cstdint>
#include <memory>

#include "varjus_api/internal/globalDefinitions.hpp"

class IConstEvalValue;


class CConstEvalVariable
{
	NONCOPYABLE(CConstEvalVariable);
public:
	CConstEvalVariable();
	~CConstEvalVariable();

	void SetValue(IConstEvalValue* v);
	[[maybe_unused]] bool Release();

	[[nodiscard]] inline auto& GetValue() noexcept { return m_pValue; }
	[[nodiscard]] constexpr CConstEvalVariable* Copy() noexcept { m_uRefCount++; return this; }
	[[nodiscard]] constexpr std::size_t& RefCount() noexcept { return m_uRefCount; }

protected:
	IConstEvalValue* m_pValue{};
	std::size_t m_uRefCount{ std::size_t(1) };
};

#endif
