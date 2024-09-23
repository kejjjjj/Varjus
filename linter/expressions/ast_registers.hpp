#pragma once

#include <array>

enum Register : std::uint8_t;
struct CRegister;
class CLinterOperand;

class CContextRegister final
{
	friend class CRegisters;
	friend class AstToInstructionConverter;
public:
	CContextRegister(const CRegister* reg) : m_pRegister(reg) {};

private:
	const CRegister* m_pRegister = 0;
	const CLinterOperand* m_pValue = 0;
};

class CRegisters final
{
public:
	CRegisters(std::array<CContextRegister, 4u>& _registers);
	~CRegisters();


	[[nodiscard]] CContextRegister* Get(Register reg) const noexcept;
	[[nodiscard]] Register AllocateRegister(Register wishRegister) const noexcept;

	[[nodiscard]] bool OperandIsLoaded(const CLinterOperand* target) const noexcept;
	[[nodiscard]] Register GetOperandRegister(const CLinterOperand* target) const noexcept;


private:

	void FreeRegister(Register reg) noexcept;

	std::array<CContextRegister, 4u>& m_oRefRegisters;
};