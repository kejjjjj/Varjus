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
struct CProgramContext;
using RuntimeFunction = std::unique_ptr<CRuntimeFunction>;


template<typename T>
concept IValueChild = std::is_base_of_v<IValue, T> || std::is_same_v<CVariable, T>;
template<typename T>
concept VariableT = std::is_same_v<CVariable, T>;

class CProgramRuntime
{
	NONCOPYABLE(CProgramRuntime);

	friend class CRuntimeFunction;
	friend class CRuntimeExpression;
	friend class CInternalArrayValue;
	friend class CAggregate;

public:
	// only one file for now
	CProgramRuntime(CFileRuntimeData* const file, CProgramContext* const context);
	~CProgramRuntime();

	void Execute();

	[[nodiscard]] static CProgramContext* GetContext();
	[[nodiscard]] static CRuntimeFunction* GetFunctionByIndex(std::size_t index);

private:

	template <IValueChild T>
	static COwningObjectPool<T> m_oValuePool;


public:

	template <IValueChild T>
	[[nodiscard]] static constexpr COwningObjectPool<T>& GetPool() {
		return m_oValuePool<T>;
	}

	[[nodiscard]] static size_t GetPoolUseCount() {
		return 
			GetPool<IValue>().GetInUseCount()
			+ GetPool<CBooleanValue>().GetInUseCount()
			+ GetPool<CIntValue>().GetInUseCount()
			+ GetPool<CDoubleValue>().GetInUseCount()
			+ GetPool<CStringValue>().GetInUseCount()
			+ GetPool<CCallableValue>().GetInUseCount();

	}

	[[nodiscard]] static CVariable* AcquireNewVariable();
	[[nodiscard]] static VectorOf<CVariable*> AcquireNewVariables(std::size_t count);
	[[nodiscard]] static void FreeVariable(CVariable* var);

	template <IValueChild T>
	[[nodiscard]] static constexpr T* AcquireNewValue() {
		auto v = GetPool<T>().Acquire();
		v->SetOwner(nullptr);
		assert(!v->HasOwner());
		return v;
	}

	template <IValueChild T, typename Ctor>
	[[nodiscard]] static constexpr T* AcquireNewValue(const Ctor& ctor) {
	
		auto v = GetPool<T>().Acquire();
		v->SetOwner(nullptr);
		assert(!v->HasOwner());

		if constexpr (std::is_same_v<IValue, T>)
			return v;
		else {
			v->MakeUnique();
			v->SetStorageValue(std::forward<const Ctor&>(ctor));
			return v;
		}
	}

	template <IValueChild T, typename Ctor>
	[[nodiscard]] static constexpr T* AcquireNewValue(Ctor&& ctor) {
		auto v = GetPool<T>().Acquire();
		v->SetOwner(nullptr);

		assert(!v->HasOwner());

		if constexpr (std::is_same_v<IValue, T>)
			return v;
		else {
			v->MakeUnique();
			v->SetStorageValue(std::forward<Ctor&&>(ctor));
			return v;
		}
	}

	template <IValueChild T>
	static constexpr void FreeValue(T* value) {
		assert(!value->HasOwner());
		GetPool<T>().Release(value);
	}

private:
	static std::vector<RuntimeFunction> m_oFunctions;
	static CProgramContext* m_pContext;
};
