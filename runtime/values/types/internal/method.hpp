#pragma once

#include "runtime/values/types/internal/builtin_methods.hpp"

namespace __method__internal {
	[[nodiscard]] IValue* CreateUndefinedVar();
}

template<class T>
class CDataTypeMethods
{
public:
	CDataTypeMethods() = default;

	[[nodiscard]] static const CBuiltInMethod<T>* FindMethod(std::size_t memberIdx) {
		return CBuiltInMethods<T>::LookupMethod(memberIdx);
	}

	[[nodiscard]] IValue* Copy() {

		if (GetMethod())
			return __method__internal::CreateUndefinedVar();

		return nullptr;
	}

	[[nodiscard]] constexpr bool IsCallable() const noexcept { return !!m_pMethod; }

	constexpr void Release() noexcept { m_pMethod = nullptr; }
	
	constexpr void SetMethod(const CBuiltInMethod<T>* v) noexcept { m_pMethod = v; }
	[[nodiscard]] constexpr auto& GetMethod() const noexcept { return m_pMethod; }

private:
	const CBuiltInMethod<T>* m_pMethod{ nullptr };
};