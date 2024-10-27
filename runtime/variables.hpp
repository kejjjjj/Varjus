
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
	auto GetValue() const noexcept { return &*m_pValue; }

protected:
	IValue* m_pValue{};
};
