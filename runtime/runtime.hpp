#pragma once

#include <string>
#include <iostream>
#include <chrono>

#include "globalDefinitions.hpp"
#include "pools/object_pool_non_owning.hpp"
#include "pools/object_pool_owning.hpp"

#include "values/types/types.hpp"

class CVariable;
class CRuntimeFunction;
class CFileRuntimeData;
class IRuntimeStructure;
class CRuntimeModule;

using RuntimeFunction = std::unique_ptr<CRuntimeFunction>;
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
	friend class CInternalArrayValue;
	friend class CAggregate;

public:
	CProgramRuntime(RuntimeModules&& modules);
	~CProgramRuntime();

	void Execute();

	static void SetExecutionPosition(const CodePosition* pos) noexcept;
	[[nodiscard]] static const CodePosition* GetExecutionPosition() noexcept;

	static inline void ThrowException() noexcept { m_bExceptionThrown = true; }
	static inline void CatchException() noexcept { m_bExceptionThrown = false; }
	[[nodiscard]] static inline bool ExceptionThrown() noexcept { return m_bExceptionThrown; }
	[[nodiscard]] static inline auto& GetExceptionValue() noexcept { return m_pExceptionValue; }

	[[nodiscard]] static CRuntimeModule* GetModuleByIndex(std::size_t index);

private:

	template <IValueChild T>
	static COwningObjectPool<T> m_oValuePool;
public:

	template <IValueChild T>
	[[nodiscard]] static constexpr COwningObjectPool<T>& GetPool() {
		return m_oValuePool<T>;
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


	steady_clock BeginExecution(CRuntimeFunction* entryFunc);

	static RuntimeModules m_oModules;
	static const CodePosition* m_pCodePosition;
	static bool m_bExceptionThrown;
	static IValue* m_pExceptionValue;
};

