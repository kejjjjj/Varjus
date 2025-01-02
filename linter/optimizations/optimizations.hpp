#pragma once

#ifdef OPTIMIZATIONS


#include "values/types/opt_types.hpp"
#include "runtime/pools/object_pool_owning.hpp"
#include "variables/opt_variables.hpp"

template<typename T>
concept IConstEvalChild = std::is_base_of_v<IConstEvalValue, T> || std::is_same_v<CConstEvalVariable, T>;

class CConstEvalVariable;

class COptimizationValues
{

private:
	template <IConstEvalChild T>
	static COwningObjectPool<T> m_oValuePool;
public:

	template <IConstEvalChild T>
	[[nodiscard]] static constexpr COwningObjectPool<T>& GetPool() {
		return m_oValuePool<T>;
	}

	[[nodiscard]] static CConstEvalVariable* AcquireNewVariable();
	[[nodiscard]] static VectorOf<CConstEvalVariable*> AcquireNewVariables(std::size_t count);
	[[nodiscard]] static void FreeVariable(CConstEvalVariable* var);

	template <IConstEvalChild T>
	[[nodiscard]] static constexpr T* AcquireNewValue() {
		auto v = GetPool<T>().Acquire();
		v->SetOwner(nullptr);
		assert(!v->HasOwner());
		return v;
	}

	template <IConstEvalChild T, typename Ctor>
	[[nodiscard]] static constexpr T* AcquireNewValue(const Ctor& ctor) {

		auto v = GetPool<T>().Acquire();
		v->SetOwner(nullptr);
		assert(!v->HasOwner());

		if constexpr (std::is_same_v<IConstEvalValue, T>)
			return v;
		else {
			v->SetStorageValue(std::forward<const Ctor&>(ctor));
			return v;
		}
	}

	template <IConstEvalChild T, typename Ctor>
	[[nodiscard]] static constexpr T* AcquireNewValue(Ctor&& ctor) {
		auto v = GetPool<T>().Acquire();
		v->SetOwner(nullptr);

		assert(!v->HasOwner());

		if constexpr (std::is_same_v<IConstEvalValue, T>)
			return v;
		else {
			v->SetStorageValue(std::forward<Ctor&&>(ctor));
			return v;
		}
	}

	template <IConstEvalChild T>
	static constexpr void FreeValue(T* value) {
		assert(!value->HasOwner());
		GetPool<T>().Release(value);
	}
};

#endif