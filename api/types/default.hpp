#pragma once

#include "globalDefinitions.hpp"
#include <cstdint>
#include <memory>
#include <string>
#include <variant>
#include <vector>
#include <cassert>

using namespace std::string_literals;

class CVariable;
class CRuntimeFunction;
class IValue;
class CBooleanValue;
class CIntValue;
class CDoubleValue;
class CStringValue;
class CInternalArrayValue;
class CArrayValue;
class CCallableValue;
class CBuiltInMemberCallableValue;
class CObjectValue;
struct CRuntimeContext;

template<typename T>
using VectorOf = std::vector<T>;
using IValues = VectorOf<IValue*>;

inline std::string emptyString;

class IValueDestructor {
public:
	virtual ~IValueDestructor() = default;
protected:
	virtual void Release() = 0;
};

class IValueCopy {
public:
	virtual ~IValueCopy() = default;
protected:
	[[nodiscard]] virtual IValue* Copy() = 0;
};

class IValue : protected IValueDestructor, protected IValueCopy
{
public:
	IValue() = default;
	virtual ~IValue() = default;

	[[nodiscard]] virtual EValueType Type() const noexcept { return t_undefined; };

	constexpr auto SetOwner(CVariable* o) noexcept { m_pOwner = o; }
	[[nodiscard]] constexpr auto GetOwner() const noexcept { return m_pOwner; }
	[[nodiscard]] constexpr auto HasOwner() const noexcept { return !!m_pOwner; }

	[[nodiscard]] virtual constexpr bool IsHanging() const noexcept { return !HasOwner(); }
	[[nodiscard]] virtual constexpr bool IsSharedObject() const noexcept { return false; }

	virtual void Release() override;
	[[nodiscard]] virtual IValue* Copy() override;

	[[nodiscard]] std::string ToPrintableString() const;

	[[nodiscard]] virtual std::string TypeAsString() const { return "undefined"s; }
	[[nodiscard]] virtual std::string ValueAsString() const { return "undefined"s; }
	

	constexpr void MakeImmutable() noexcept { m_bIsConst = true; }

	[[nodiscard]] constexpr virtual bool IsImmutable() const noexcept { return m_bIsConst; }
	[[nodiscard]] constexpr virtual bool IsCoerceable() const noexcept { return false; }
	[[nodiscard]] constexpr virtual bool IsArithmetic() const noexcept { return false; }
	[[nodiscard]] constexpr virtual bool IsIntegral() const noexcept { return false; }
	[[nodiscard]] constexpr virtual bool IsIndexable() const noexcept { return false; }
	[[nodiscard]] constexpr virtual bool IsCallable() const noexcept { return false; }
	[[nodiscard]] constexpr virtual bool IsAggregate() const noexcept { return false; }
	[[nodiscard]] constexpr virtual bool IsBooleanConvertible() const noexcept { return true; }
	[[nodiscard]] constexpr virtual bool IsBuiltInMemberCallable() const noexcept { return false; }

	[[nodiscard]] virtual IValue* Index(std::int64_t index);
	[[nodiscard]] virtual IValue* GetAggregate([[maybe_unused]]std::size_t memberIdx) { return nullptr; }
	[[nodiscard]] virtual IValue* Call([[maybe_unused]] struct CRuntimeContext* const ctx, [[maybe_unused]] const IValues& args) {
		return nullptr;
	}
	[[nodiscard]] bool& AsBoolean();
	[[nodiscard]] std::int64_t& AsInt();
	[[nodiscard]] double &AsDouble();
	[[nodiscard]] std::string& AsString();

	[[nodiscard]] virtual bool ToBoolean() const { return false; }
	[[nodiscard]] virtual std::int64_t ToInt() const { return 0; }
	[[nodiscard]] virtual double ToDouble() const { return 0.0; }
	[[nodiscard]] virtual const std::string& ToString() const { return emptyString; }

	[[nodiscard]] virtual std::size_t AddressOf() const noexcept { return reinterpret_cast<std::size_t>(this); }

	[[nodiscard]] virtual CBooleanValue* ToCBoolean() { return nullptr; }
	[[nodiscard]] virtual CIntValue* ToCInt() { return nullptr; }
	[[nodiscard]] virtual CDoubleValue* ToCDouble() { return nullptr; }

	[[nodiscard]] virtual CStringValue* ToCString() { return nullptr; }
	[[nodiscard]] virtual CCallableValue* ToCallable() { return nullptr; }
	[[nodiscard]] virtual CBuiltInMemberCallableValue* ToBuiltInMemberCallable() { return nullptr; }

	[[nodiscard]] virtual CArrayValue* ToArray() { return nullptr; }
	[[nodiscard]] virtual CObjectValue* ToObject() { return nullptr; }

protected:
	void ReleaseInternal();
	void RemoveConstness() noexcept { m_bIsConst = false; }

	CVariable* m_pOwner{ nullptr };
	bool m_bIsConst{ false };
};

template <typename Value>
class CValue : public IValue {
	static_assert(std::is_default_constructible_v<Value>, "T must be default constructible");
public:
	// no reason to have other constructors as these objects are created before their actually needed use!
	CValue() = default;
	
	virtual ~CValue() = default;

	constexpr void MakeShared() {
		m_bShared = true;
		m_oValue = std::make_shared<Value>();
	}
	constexpr void MakeUnique() {
		m_bShared = false;
		m_oValue = Value{};
	}
	[[nodiscard]] constexpr std::shared_ptr<Value>& GetShared() {
		assert(m_bShared);
		return std::get<1>(m_oValue);
	}
	[[nodiscard]] constexpr const std::shared_ptr<Value>& GetShared() const {
		assert(m_bShared);
		return std::get<1>(m_oValue);
	}

	[[nodiscard]] constexpr Value& Get() {
		if (m_bShared)
			return *std::get<1>(m_oValue);
		return std::get<0>(m_oValue);
	}
	[[nodiscard]] constexpr const Value& Get() const {
		if (m_bShared)
			return *std::get<1>(m_oValue);
		return std::get<0>(m_oValue);
	}

	void SetStorageValue(Value&& v) { m_oValue = std::move(v); }
	void SetStorageValue(const Value& v) { m_oValue = v; }

	[[nodiscard]] constexpr auto IsShared() const noexcept { return m_bShared; }
	[[nodiscard]] constexpr bool IsHanging() const noexcept override { 

		if (!IValue::HasOwner() && !IsShared())
			return true;

		return !IValue::HasOwner() && SharedRefCount() == 1;
	}

	[[nodiscard]] constexpr bool IsSharedObject() const noexcept override { return IsShared(); }

protected:
	std::variant<Value, std::shared_ptr<Value>> m_oValue{};
	bool m_bShared{ false };

	[[nodiscard]] constexpr auto SharedRefCount() const noexcept { return GetShared().use_count(); }

	[[maybe_unused]] void ReleaseShared() {
		assert(m_bShared);
		GetShared().reset();
	}

private:

};
