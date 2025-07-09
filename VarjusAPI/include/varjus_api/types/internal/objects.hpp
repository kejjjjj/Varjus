#pragma once

#include "varjus_api/types/object.hpp"

#include <vector>
#include <memory>
#include <functional>
#include <optional>

namespace Varjus {
	struct BuiltInMethod_t;
	struct BuiltInProperty_t;
	struct ObjectDeclaration_t;

	class CBuiltInObject final : public CObjectValue
	{
		VARJUS_NONCOPYABLE(CBuiltInObject);

	public:
		CBuiltInObject();
		~CBuiltInObject();
		static CBuiltInObject* Construct(Varjus::CProgramRuntime* const runtime, Varjus::BuiltInMethod_t&& methods, Varjus::BuiltInProperty_t&& properties);

		void Release() override;

		[[nodiscard]] const constexpr CBuiltInObject* ToBuiltInObject() const noexcept override { return this; }
		[[nodiscard]] constexpr CBuiltInObject* ToBuiltInObject() noexcept override { return this; }

		[[nodiscard]] IValue* Copy() override;
		[[nodiscard]] IValue* Index(Varjus::CRuntimeContext* const ctx, IValue* index) override;
		[[nodiscard]] IValue* GetAggregate(Varjus::CRuntimeContext* const ctx, std::size_t memberIdx) override;

		[[nodiscard]] auto GetMethods() const noexcept { return m_oMethods.get(); }
		[[nodiscard]] auto GetProperties() const noexcept { return m_oProperties.get(); }

	private:
		[[nodiscard]] VarjusString ValueAsString() const override;
		[[nodiscard]] VarjusString ValueAsEscapedString() const override { return CBuiltInObject::ValueAsString(); }

		std::shared_ptr<Varjus::BuiltInMethod_t> m_oMethods;
		std::shared_ptr<Varjus::BuiltInProperty_t> m_oProperties;
	};

	template<typename Type> requires std::is_same_v<void, std::remove_reference_t<Type>>
	using OptionalCtor = std::optional<std::function<Type(Varjus::ObjectDeclaration_t&)>>;

	struct CBuiltInObjectPairs
	{
		~CBuiltInObjectPairs();
		OptionalCtor<void> constructor;
	};

	class CBuiltInObjects {
	public:
		void AddNewStaticObject(const VarjusString& name, const OptionalCtor<void>& constructorFunc = std::nullopt);
		void Reset() noexcept { m_arrData.clear(); }
		[[nodiscard]] auto& Iterator() noexcept { return m_arrData; }
	private:
		std::vector<std::pair<VarjusString, CBuiltInObjectPairs>> m_arrData;

	};
}