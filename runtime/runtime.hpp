#pragma once

#include <string>
#include <iostream>

#include "globalDefinitions.hpp"
#include "pools/object_pool_non_owning.hpp"
#include "pools/object_pool_owning.hpp"

#include "values/types/types.hpp"

class CVariable;
class CRuntimeFunction;
class CFileRuntimeData;

using RuntimeFunction = std::unique_ptr<CRuntimeFunction>;


template<typename T>
concept IValueChild = std::is_base_of_v<IValue, T>;

class CProgramRuntime
{
	NONCOPYABLE(CProgramRuntime);

	friend class CRuntimeFunction;
	friend class CRuntimeExpression;
	friend class CInternalArrayValue;

public:
	// only one file for now
	CProgramRuntime(CFileRuntimeData* const file);
	~CProgramRuntime();

	void Execute();

	[[nodiscard]] static CRuntimeFunction* GetFunctionByIndex(std::size_t index);

private:

	template <IValueChild T>
	static COwningObjectPool<T> m_oValuePool;


public:

	template <IValueChild T>
	static constexpr COwningObjectPool<T>& GetPool() {
		return m_oValuePool<T>;
	}

	static size_t GetPoolUseCount() {
		return 
			GetPool<IValue>().GetInUseCount()
			+ GetPool<CBooleanValue>().GetInUseCount()
			+ GetPool<CIntValue>().GetInUseCount()
			+ GetPool<CDoubleValue>().GetInUseCount()
			+ GetPool<CStringValue>().GetInUseCount()
			+ GetPool<CCallableValue>().GetInUseCount();

	}

	template <IValueChild T>
	static constexpr T* AcquireNewValue() {
		//std::cout << std::format("{}A {}\n",
		//	std::string(GetPool<T>().GetInUseCount(), '-'), typeid(T).name());

		auto v = GetPool<T>().Acquire();
		v->SetOwner(nullptr);
		assert(!v->HasOwner());
		return v;
	}

	template <IValueChild T, typename Ctor>
	static constexpr T* AcquireNewValue(const Ctor& ctor) {
		
		//std::cout << std::format("{}A {}\n",
		//	std::string(GetPool<T>().GetInUseCount(), '-'), typeid(T).name());

		auto v = GetPool<T>().Acquire();
		v->SetOwner(nullptr);
		assert(!v->HasOwner());

		if constexpr (std::is_same_v<IValue, T>)
			return v;
		else {
			v->SetRawValue(std::forward<const Ctor&>(ctor));
			return v;
		}
	}

	template <IValueChild T, typename Ctor>
	static constexpr T* AcquireNewValue(Ctor&& ctor) {
		//std::cout << std::format("{}A {}\n",
		//	std::string(GetPool<T>().GetInUseCount(), '-'), typeid(T).name());

		auto v = GetPool<T>().Acquire();
		v->SetOwner(nullptr);
		assert(!v->HasOwner());

		if constexpr (std::is_same_v<IValue, T>)
			return v;
		else {
			v->SetRawValue(std::forward<Ctor&&>(ctor));
			return v;
		}
	}

	template <IValueChild T>
	static constexpr void FreeValue(T* value) {
		assert(!value->HasOwner());
		GetPool<T>().Release(value);
		//std::cout << std::format("{}D {}\n",
		//	std::string(GetPool<T>().GetInUseCount(), '-'), typeid(T).name());
	}

private:
	static CNonOwningObjectPool<CVariable> m_oVariablePool;
	static std::vector<RuntimeFunction> m_oFunctions;
};
