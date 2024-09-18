#pragma once
#include <vector>
#include <string>


struct CLinterVariable;

enum OperandType : std::uint8_t
{
	o_register,
	o_immediate,
	o_variable,
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
	constexpr CIntOperand(std::int64_t v) : m_iValue(v){}
	~CIntOperand() = default;

	[[nodiscard]] constexpr OperandType Type() const noexcept override { return o_immediate; }

	[[nodiscard]] std::string ToString() const noexcept override { return std::to_string(m_iValue); }
private:
	std::int64_t m_iValue{};
};

class CVariableOperand : public COperand
{
public:
	CVariableOperand() = delete;
	constexpr CVariableOperand(std::size_t vId, const CLinterVariable* var) : m_iVariableIndex(vId), m_pVariable(var){}
	~CVariableOperand() = default;

	[[nodiscard]] constexpr OperandType Type() const noexcept override { return o_variable; }
	[[nodiscard]] std::string ToString() const noexcept override { return "[EBP + " + std::to_string(m_iVariableIndex) + "]"; }


private:
	std::size_t m_iVariableIndex{};
	const CLinterVariable* m_pVariable{};
};
