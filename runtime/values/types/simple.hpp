#pragma once

#include "globalDefinitions.hpp"
#include <cstdint>
#include <memory>
#include <string>

using namespace std::string_literals;

class CVariable;
class CRuntimeFunction;
class IValue;
class CInternalArrayValue;
class CArrayValue;
class CObjectValue;

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
	[[nodiscard]] constexpr virtual bool IsBooleanConvertible() const noexcept { return false; }

	[[nodiscard]] virtual IValue* Index(std::int64_t index);
	[[nodiscard]] virtual IValue* GetAggregate([[maybe_unused]]std::size_t memberIdx) { return nullptr; }

	[[nodiscard]] bool& AsBoolean();
	[[nodiscard]] std::int64_t& AsInt();
	[[nodiscard]] double &AsDouble();
	[[nodiscard]] std::string& AsString();
	[[nodiscard]] CRuntimeFunction* AsCallable();
	[[nodiscard]] CInternalArrayValue* AsArray();

	[[nodiscard]] virtual bool ToBoolean() const { return false; }
	[[nodiscard]] virtual std::int64_t ToInt() const { return 0; }
	[[nodiscard]] virtual double ToDouble() const { return 0.0; }
	[[nodiscard]] virtual const std::string& ToString() const { return emptyString; }

	[[nodiscard]] virtual CArrayValue* ToArray() { return nullptr; }
	[[nodiscard]] virtual CObjectValue* ToObject() { return nullptr; }

protected:
	void ReleaseInternal();
	void RemoveConstness() noexcept { m_bIsConst = false; }

	CVariable* m_pOwner{ nullptr };
	bool m_bIsConst{ false };
};

template <typename Value>
struct DirectStorage {
	using StorageType = Value;
	StorageType m_oValue;

	DirectStorage() = default;
	explicit DirectStorage(const Value& value) : m_oValue(value) {}
	explicit DirectStorage(Value&& value) : m_oValue(std::move(value)) {}

	constexpr Value& Get() { return m_oValue; }
	constexpr const Value& Get() const { return m_oValue; }

	void SetStorageValue(Value&& v) { m_oValue = std::move(v); }
	void SetStorageValue(const Value& v) { m_oValue = v; }

};

template <typename Value>
struct SharedStorage {
	using StorageType = std::shared_ptr<Value>;
	StorageType m_oValue;

	SharedStorage() : m_oValue(std::make_shared<Value>()) {}
	explicit SharedStorage(const Value& value) : m_oValue(std::make_shared<Value>(value)) {}
	explicit SharedStorage(Value&& value) : m_oValue(std::make_shared<Value>(std::move(value))) {}

	Value& Get() { return m_oValue; }
	const Value& Get() const { return m_oValue; }

	void SetStorageValue(Value&& v) { m_oValue = std::move(v); }
	void SetStorageValue(const Value& v) { m_oValue = v; }
};

template <typename Value, template <typename> class StoragePolicy = DirectStorage>
class CValue : public IValue, public StoragePolicy<Value> {
protected:
	using StoragePolicy<Value>::m_oValue;

public:
	CValue() = default;
	explicit CValue(const Value& value) : StoragePolicy<Value>(value) {}
	explicit CValue(Value&& value) : StoragePolicy<Value>(std::move(value)) {}
	virtual ~CValue() = default;
};

// Generic wrapper for shared ownership
template <typename Derived, typename Value>
class CSharedValue : public Derived {
public:
	using Base = Derived;
	using StorageType = SharedStorage<Value>;

	CSharedValue() = default;
	explicit CSharedValue(const Value& value) : Base(value) {
		static_assert(std::is_base_of_v<CValue<Value, SharedStorage>, Derived>,
			"Derived must inherit from CValue<Value, SharedStorage>");
	}
	explicit CSharedValue(Value&& value) : Base(std::move(value)) {}
};

template<typename T>
using SharedCValue = CValue<T, SharedStorage>;

