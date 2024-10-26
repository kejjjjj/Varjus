
#include <cstddef>
#include <cstdint>
#include <memory>

#include "globalDefinitions.hpp"

class IValue;

enum EValueType
{
	t_undefined,
	t_int,
	t_double
};

class CVariable
{
	NONCOPYABLE(CVariable);
public:
	CVariable(std::size_t index) : m_pValue(std::make_unique<IValue>()), m_uIndex(index){}
protected:
	std::unique_ptr<IValue> m_pValue;
	std::size_t m_uIndex{};
};

class IValue
{
	NONCOPYABLE(IValue);
public:
	[[nodiscard]] virtual EValueType Type() const noexcept { return t_undefined; };
};

class CIntValue : public IValue
{
	NONCOPYABLE(CIntValue);
public:
	[[nodiscard]] virtual EValueType Type() const noexcept { return t_int; };

private:
	std::int64_t m_iValue;
};
