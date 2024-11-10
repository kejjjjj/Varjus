#include "shared_ownership.hpp"

#include <cassert>

CSharedValueOwnership::CSharedValueOwnership(IValue* value) 
	: m_pOwner(value), m_uRefCount(new std::size_t(1)){}

void CSharedValueOwnership::Release()
{
	assert(m_uRefCount);
	assert(*m_uRefCount > 0);

	if (--(*m_uRefCount) == std::size_t(0)) {
		m_pOwner->Release();
		delete (m_uRefCount = nullptr);
	}
}

IValue* CSharedValueOwnership::Copy()
{
	assert(m_uRefCount);

	(*m_uRefCount)++;
	return m_pOwner;
}

std::size_t CSharedValueOwnership::GetRefCount() const noexcept
{
	assert(m_uRefCount);
	return *m_uRefCount;
}
IValue* CSharedValueOwnership::GetValue() noexcept
{
	assert(m_pOwner);
	return m_pOwner;
}
