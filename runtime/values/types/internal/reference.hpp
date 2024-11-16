#pragma once

#include <memory>

#include "runtime/values/types/types.hpp"
#include "runtime/runtime.hpp"

template<typename T>
concept BaseOfIValue = std::is_base_of_v<IValue, T>;

template<BaseOfIValue UnderlyingType>
class CReferenceValue
{
	using SharedPtr = std::shared_ptr<UnderlyingType>;
public:

	void Release() {
		if (m_oValue.use_count() == 1)
			m_oValue->Release();

		m_oValue->ReleaseInternal();
		CProgramRuntime::FreeValue<UnderlyingType>(this);
		m_oValue.reset();
	}
	
	SharedPtr m_oValue;
};
