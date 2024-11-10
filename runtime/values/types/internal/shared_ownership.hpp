#pragma once

#include "runtime/values/types/simple.hpp"


class CSharedValueOwnership : protected IValueDestructor, protected IValueCopy
{
public:
	explicit CSharedValueOwnership(IValue* value = nullptr);
	void Release() override;
	[[nodiscard]] IValue* Copy() override;

	[[nodiscard]] std::size_t GetRefCount() const noexcept;
	[[nodiscard]] IValue* GetValue() noexcept;
protected:

private:
	IValue* m_pOwner{};
	std::size_t* m_uRefCount{};
};
