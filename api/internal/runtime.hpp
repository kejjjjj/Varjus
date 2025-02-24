#pragma once

#include <string>
#include <iostream>
#include <chrono>

#include "api/internal/globalDefinitions.hpp"
#include "api/types/types.hpp"
#include "api/types/internal/objects.hpp"
#include "variables.hpp"
#include "pools/object_pool_owning.hpp"
#include <typeindex>

class CRuntimeFunction;
class CRuntimeFunctionBase;
class CFileRuntimeData;
class IRuntimeStructure;
class CRuntimeModule;

using RuntimeFunction = std::unique_ptr<CRuntimeFunctionBase>;
using CodePosition = std::tuple<size_t, size_t>;
using RuntimeBlock = std::unique_ptr<IRuntimeStructure>;
using steady_clock = std::chrono::time_point<std::chrono::steady_clock>;

template<typename T>
concept IValueChild = std::is_base_of_v<IValue, T> || std::is_same_v<CVariable, T>;
template<typename T>
concept VariableT = std::is_same_v<CVariable, T>;

using RuntimeModules = VectorOf<std::unique_ptr<CRuntimeModule>>;

class CProgramRuntime
{
	NONCOPYABLE(CProgramRuntime);

	friend class CRuntimeFunction;
	friend class CRuntimeExpression;

public:
	CProgramRuntime(RuntimeModules&& modules);
	~CProgramRuntime();

	IValue* Execute();
	void Cleanup();

	void SetExecutionPosition(const CodePosition* pos) noexcept;
	[[nodiscard]] const CodePosition* GetExecutionPosition() noexcept;

	inline void ThrowException() noexcept { m_bExceptionThrown = true; }
	inline void CatchException() noexcept { m_bExceptionThrown = false; }
	[[nodiscard]] inline bool ExceptionThrown() noexcept { return m_bExceptionThrown; }
	[[nodiscard]] inline auto& GetExceptionValue() noexcept { return m_pExceptionValue; }

	[[nodiscard]] CRuntimeModule* GetModuleByIndex(std::size_t index);
	[[nodiscard]] bool HasLeaks();
	void PrintAllLeaks();

private:

	std::tuple<
		COwningObjectPool<CVariable>,
		COwningObjectPool<IValue>,
		COwningObjectPool<CBooleanValue>,
		COwningObjectPool<CIntValue>,
		COwningObjectPool<CUIntValue>,
		COwningObjectPool<CDoubleValue>,
		COwningObjectPool<CStringValue>,
		COwningObjectPool<CCallableValue>,
		COwningObjectPool<CArrayValue>,
		COwningObjectPool<CObjectValue>,
		COwningObjectPool<CBuiltInObject>
	>m_oValuePools;

	template <typename T, std::size_t... Is>
	[[nodiscard]] static constexpr std::size_t GetPoolIndexImpl(std::index_sequence<Is...>) {
		return ((std::is_same_v<T, std::tuple_element_t<Is, decltype(m_oValuePools)>> ? Is : 0) + ...);
	}

	template <typename T>
	[[nodiscard]] static constexpr std::size_t GetPoolIndex() {
		return GetPoolIndexImpl<T>(std::make_index_sequence<std::tuple_size_v<decltype(m_oValuePools)>>{});
	}

public:

	template <typename T>
	[[nodiscard]] constexpr COwningObjectPool<T>& GetPool() {
		return std::get<GetPoolIndex<COwningObjectPool<T>>()>(m_oValuePools);
	}

	void AllocatePools(std::size_t initialSize) {
		std::apply([initialSize](auto&... pools) {
			(..., pools.Grow(initialSize));
		}, m_oValuePools);
	}

	[[nodiscard]] CVariable* AcquireNewVariable();
	[[nodiscard]] VectorOf<CVariable*> AcquireNewVariables(std::size_t count);
	void FreeVariable(CVariable* var);

	template <IValueChild T>
	[[nodiscard]] constexpr T* AcquireNewValue() {
		auto v = GetPool<T>().Acquire();
		v->SetOwner(nullptr);
		v->ConstructInternal(this);

		assert(!v->HasOwner());
		return v;
	}

	template <IValueChild T, typename Ctor>
	[[nodiscard]] constexpr T* AcquireNewValue(const Ctor& ctor) {
	
		auto v = GetPool<T>().Acquire();
		v->SetOwner(nullptr);
		v->ConstructInternal(this);
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
	[[nodiscard]] constexpr T* AcquireNewValue(Ctor&& ctor) {
		auto v = GetPool<T>().Acquire();
		v->SetOwner(nullptr);
		v->ConstructInternal(this);

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
	constexpr void FreeValue(T* value) {
		assert(!value->HasOwner());
		value->m_pAllocator = nullptr;
		GetPool<T>().Release(value);
	}

private:

	[[nodiscard]] IValue* BeginExecution(CRuntimeFunction* entryFunc);
	[[nodiscard]] static CRuntimeFunction* FindMainFunction(const RuntimeModules& modules);

	void FreeAllPools();

	RuntimeModules m_oModules;
	const CodePosition* m_pCodePosition;
	bool m_bExceptionThrown;
	IValue* m_pExceptionValue;
};

