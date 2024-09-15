#pragma once
#include <vector>
#include <string>

enum OperandType : std::uint8_t
{
	o_register,
	o_immediate,
};

class COperand
{
public:
	constexpr COperand() = default;
	virtual ~COperand() = default;

	[[nodiscard]] constexpr virtual OperandType Type() const noexcept = 0;
	[[nodiscard]] virtual std::string ToString() const noexcept = 0;
	
	[[nodiscard]] std::byte SizeOf() const noexcept { return m_uSize; }

protected:
	std::byte m_uSize{ 0 };
};

class CIntOperand : public COperand
{
public:
	CIntOperand() = delete;
	constexpr CIntOperand(std::int64_t v) : m_iValue(v) { m_uSize = static_cast<std::byte>(sizeof(m_iValue)); }
	~CIntOperand() = default;

	[[nodiscard]] constexpr OperandType Type() const noexcept override { return o_immediate; }

	[[nodiscard]] std::string ToString() const noexcept override { return std::to_string(m_iValue); }
private:
	std::int64_t m_iValue{};
};
