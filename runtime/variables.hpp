
#include <cstddef>
#include <cstdint>
#include <memory>

#include "globalDefinitions.hpp"

class IValue;


class CVariable
{
	NONCOPYABLE(CVariable);
public:
	CVariable();
	~CVariable();

	void SetValue(IValue* v);
	[[maybe_unused]] bool Release();

	[[nodiscard]] inline auto& GetValue() noexcept { return m_pValue; }
	[[nodiscard]] constexpr CVariable* Copy() noexcept { m_uRefCount++; return this; }
	[[nodiscard]] constexpr std::size_t& RefCount() noexcept { return m_uRefCount; }

	bool m_bSelfCapturing{ false }; //not exactly the most efficient way to include this here

protected:
	IValue* m_pValue{};
	std::size_t m_uRefCount{std::size_t(1)};
};
