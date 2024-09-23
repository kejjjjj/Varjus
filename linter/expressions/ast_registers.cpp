#include "ast_registers.hpp"
#include "operand.hpp"
#include "identifier.hpp"
#include "linter/token.hpp"
#include <cassert>
#include "instructions/register.hpp"

CRegisters::CRegisters(std::array<CContextRegister, 4u>& _registers) : m_oRefRegisters(_registers){}
CRegisters::~CRegisters() = default;

CContextRegister* CRegisters::Get(Register reg) const noexcept
{
	return &m_oRefRegisters[reg];
}

Register CRegisters::AllocateRegister(Register wishRegister) const noexcept {

	if (wishRegister != ANY_REGISTER) {
		if (!m_oRefRegisters[wishRegister].m_pValue)
			return m_oRefRegisters[wishRegister].m_pRegister->m_eRegister;
	}

	for (const auto& preg : m_oRefRegisters) {
		if (!preg.m_pValue)
			return preg.m_pRegister->m_eRegister;
	}

	assert(false);
	return EAX;
}
void CRegisters::FreeRegister(Register reg) noexcept
{
	m_oRefRegisters[reg].m_pValue = nullptr;
}

bool CRegisters::OperandIsLoaded(const CLinterOperand* target) const noexcept
{
	for (const auto& preg : m_oRefRegisters) {
		if (preg.m_pValue == target)
			return true;
	}

	return false;
}
Register CRegisters::GetOperandRegister(const CLinterOperand* target) const noexcept
{

	for (const auto& preg : m_oRefRegisters) {
		if (preg.m_pValue == target)
			return preg.m_pRegister->m_eRegister;
	}

	assert(false);
	return EAX;
}
