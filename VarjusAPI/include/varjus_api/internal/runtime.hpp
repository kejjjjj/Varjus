#pragma once


#include <typeindex>
#include <atomic>

#include "varjus_api/internal/globalDefinitions.hpp"
#include "varjus_api/types/types.hpp"
#include "varjus_api/types/internal/objects.hpp"
#include "variables.hpp"
#include "pools/object_pool_owning.hpp"


class CRuntimeFunction;
class CRuntimeFunctionBase;
class CFileRuntimeData;
class IRuntimeStructure;

namespace Varjus
{
	class CRuntimeModule;
	class CProgramInformation;
}

using __RuntimeFunction = std::unique_ptr<CRuntimeFunctionBase>;
using __CodePosition = std::tuple<size_t, size_t>;
using __RuntimeBlock = std::unique_ptr<IRuntimeStructure>;

template<typename T>
concept IValueChild = std::is_base_of_v<IValue, T> || std::is_same_v<CVariable, T> || std::is_same_v<CChildVariable, T>;
template<typename T>
concept VariableT = std::is_same_v<CVariable, T> || std::is_same_v<CChildVariable, T>;

using RuntimeModules = VectorOf<std::unique_ptr<Varjus::CRuntimeModule>>;

namespace Varjus {

	template<typename IValueChild>
	struct CDefaultObject final {

		//VARJUS_NONCOPYABLE(CDefaultObject);
		CDefaultObject(std::unique_ptr<Varjus::BuiltInMethod_t>&& methods, std::unique_ptr<Varjus::BuiltInProperty_t>&& properties)
			: m_oMethods(std::move(methods)), m_oProperties(std::move(properties)) {
		}

		[[nodiscard]] constexpr auto GetMethods() { return m_oMethods.get(); }
		[[nodiscard]] constexpr auto GetProperties() { return m_oProperties.get(); }

	private:
		std::unique_ptr<Varjus::BuiltInMethod_t> m_oMethods;
		std::unique_ptr<Varjus::BuiltInProperty_t> m_oProperties;
	};

	class CProgramRuntime
	{
		VARJUS_NONCOPYABLE(CProgramRuntime);

		friend class CRuntimeFunction;
		friend class CRuntimeModule;

	public:
		CProgramRuntime(std::unique_ptr<CProgramInformation>&& information, RuntimeModules&& modules);
		~CProgramRuntime();

		[[nodiscard]] IValue* Execute(IValues& args);
		void Cleanup();

		void SetExecutionPosition(const __CodePosition* pos) noexcept;
		[[nodiscard]] const __CodePosition* GetExecutionPosition() noexcept;

		inline void ThrowException() noexcept { m_bExceptionThrown = true; }
		inline void CatchException() noexcept { m_bExceptionThrown = false; }
		[[nodiscard]] inline bool ExceptionThrown() const noexcept { return m_bExceptionThrown; }
		[[nodiscard]] inline auto& GetExceptionValue() noexcept { return m_pExceptionValue; }

		[[nodiscard]] CRuntimeModule* GetModuleByIndex(std::size_t index);
		[[nodiscard]] bool HasLeaks();
		void PrintAllLeaks();

		inline void Abort() noexcept { m_bAbort.store(true); }
		[[nodiscard]] inline bool WaitingToAbort() const noexcept { return m_bAbort.load(); }

	private:
		//very important that I am initialized before m_oValuePools
		std::unique_ptr<CProgramInformation> m_pInformation;

		std::tuple<
			COwningObjectPool<CVariable>,
			COwningObjectPool<CChildVariable>,
			COwningObjectPool<IValue>,
			COwningObjectPool<CBooleanValue>,
			COwningObjectPool<CIntValue>,
			COwningObjectPool<CUIntValue>,
			COwningObjectPool<CDoubleValue>,
			COwningObjectPool<CStringValue>,
			COwningObjectPool<CCallableValue>,
			COwningObjectPool<CArrayValue>,
			COwningObjectPool<CObjectValue>,
			COwningObjectPool<Varjus::CBuiltInObject>
		>m_oValuePools;

		std::tuple<
			CDefaultObject<CStringValue>,
			CDefaultObject<CArrayValue>,
			CDefaultObject<CObjectValue>
		>m_oDefaultObjects;


		template <typename T, typename TupleType, std::size_t... Is>
		[[nodiscard]] static constexpr std::size_t GetTupleIndex(std::index_sequence<Is...>) {
			return ((std::is_same_v<T, std::tuple_element_t<Is, TupleType>> ? Is : 0) + ...);
		}

		template <typename T, typename TupleType>
		[[nodiscard]] static constexpr std::size_t GetIndex() {
			return GetTupleIndex<T, TupleType>(std::make_index_sequence<std::tuple_size_v<TupleType>>{});
		}

	public:

		template <typename T>
		[[nodiscard]] constexpr COwningObjectPool<T>& GetPool() {
			return std::get<GetIndex<COwningObjectPool<T>, decltype(m_oValuePools)>()>(m_oValuePools);
		}

		template <typename T>
		[[nodiscard]] constexpr CDefaultObject<T>& GetDefaultObject() {
			return std::get<GetIndex<CDefaultObject<T>, decltype(m_oDefaultObjects)>()>(m_oDefaultObjects);
		}

		void AllocatePools(std::size_t initialSize) {
			std::apply([initialSize](auto&... pools) {
				(..., pools.Grow(initialSize));
				}, m_oValuePools);
		}

		[[nodiscard]] CVariable* AcquireNewVariable();
		[[nodiscard]] VectorOf<CVariable*> AcquireNewVariables(std::size_t count);
		void FreeVariable(CVariable* var);

		[[nodiscard]] CChildVariable* AcquireNewChildVariable(const ArrayOwner& parent);
		[[nodiscard]] VectorOf<CChildVariable*> AcquireNewChildVariables(std::size_t count, const ArrayOwner& parent);
		void FreeChildVariable(CChildVariable* var);

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

		[[nodiscard]] inline auto GetInformation() const noexcept { return m_pInformation.get(); }
		[[nodiscard]] VarjusString KeyToString(__ElementIndex index) const noexcept;
		[[nodiscard]] std::size_t StringToKey(const VarjusString& key) const noexcept;
		[[nodiscard]] bool ContainsKey(const VarjusString& key) const noexcept;

	private:

		[[nodiscard]] IValue* BeginExecution(CRuntimeFunction* entryFunc, IValues& args);
		[[nodiscard]] static CRuntimeFunction* FindMainFunction(const RuntimeModules& modules);

		void FreeAllPools();

		RuntimeModules m_oModules;
		const __CodePosition* m_pCodePosition{};
		IValue* m_pExceptionValue{};
		bool m_bExceptionThrown{};
		std::atomic_bool m_bAbort{};
	};

}