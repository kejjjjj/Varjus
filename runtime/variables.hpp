
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

	[[nodiscard]] auto& GetValue() noexcept { return m_pValue; }
	[[nodiscard]] CVariable* Copy() noexcept { m_uRefCount++; return this; }
	[[nodiscard]] std::size_t& RefCount() noexcept { return m_uRefCount; }
protected:
	IValue* m_pValue{};
	std::size_t m_uRefCount{std::size_t(1)};
};
