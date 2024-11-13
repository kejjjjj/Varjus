
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

	[[nodiscard]] auto& GetValue() noexcept { return m_pValue; }

	void Release();

protected:
	IValue* m_pValue{};
};
