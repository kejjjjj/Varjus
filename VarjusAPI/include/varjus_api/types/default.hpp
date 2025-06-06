#pragma once

#include "varjus_api/internal/globalDefinitions.hpp"
#include <cstdint>
#include <memory>

#include <variant>
#include <vector>
#include <cassert>

using namespace std::string_literals;

class CVariable;
class CRuntimeFunction;
class IValue;
class CBooleanValue;
class CIntValue;
class CUIntValue;
class CDoubleValue;
class CStringValue;
class CInternalArrayValue;
class CArrayValue;
class CCallableValue;
class CObjectValue;
struct CRuntimeContext;
class CProgramRuntime;

template<typename T>
using VectorOf = std::vector<T>;
using IValues = VectorOf<IValue*>;

inline VarjusString emptyString;

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
	friend class CProgramRuntime;
public:
	IValue() = default;
	virtual ~IValue() = default;

	[[nodiscard]] static IValue* Construct(CProgramRuntime* const runtime);

	[[nodiscard]] virtual EValueType Type() const noexcept { return t_undefined; };

	constexpr auto SetOwner(CVariable* o) noexcept { m_pOwner = o; }
	[[nodiscard]] constexpr auto GetOwner() const noexcept { return m_pOwner; }
	[[nodiscard]] constexpr auto HasOwner() const noexcept { return !!m_pOwner; }

	[[nodiscard]] virtual constexpr bool IsHanging() const noexcept { return !HasOwner(); }
	[[nodiscard]] virtual constexpr bool IsSharedObject() const noexcept { return false; }

	virtual void Release() override;
	[[nodiscard]] virtual IValue* Copy() override;

	[[nodiscard]] VarjusString ToPrintableString() const;

	[[nodiscard]] virtual VarjusString TypeAsString() const { return VSL("undefined"); }
	[[nodiscard]] virtual VarjusString ValueAsString() const { return VSL("undefined"); }

	//removes the surrounding quotes (for strings)
	[[nodiscard]] virtual VarjusString ValueAsEscapedString() const { return ValueAsString(); }


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
	[[nodiscard]] constexpr virtual bool IsIterable() const noexcept { return false; }

	[[nodiscard]] virtual IValue* Index(IValue* index);
	[[nodiscard]] virtual IValue* GetAggregate([[maybe_unused]] struct CRuntimeContext* const ctx, [[maybe_unused]] std::size_t memberIdx) { return nullptr; }
	[[nodiscard]] virtual IValue* Call([[maybe_unused]] struct CRuntimeContext* const ctx, [[maybe_unused]] const IValues& args) { return nullptr; }
	
	//returns a copy, remember to free!
	[[nodiscard]] virtual IValues ToIterable() const { return {}; }

	[[nodiscard]] bool& AsBoolean();
	[[nodiscard]] VarjusInt& AsInt();
	[[nodiscard]] VarjusUInt& AsUInt();
	[[nodiscard]] VarjusDouble &AsDouble();
	[[nodiscard]] VarjusString& AsString();

	[[nodiscard]] virtual bool ToBoolean() const { return false; }
	[[nodiscard]] virtual VarjusInt ToInt() const { return VarjusInt(0); }
	[[nodiscard]] virtual VarjusUInt ToUInt() const { return VarjusUInt(0); }
	[[nodiscard]] virtual VarjusDouble ToDouble() const { return VarjusDouble(0.0); }
	[[nodiscard]] virtual const VarjusString& ToString() const { return emptyString; }

	[[nodiscard]] virtual std::size_t AddressOf() const noexcept { return reinterpret_cast<std::size_t>(this); }

	[[nodiscard]] virtual CBooleanValue* ToCBoolean() { return nullptr; }
	[[nodiscard]] virtual CIntValue* ToCInt() { return nullptr; }
	[[nodiscard]] virtual CUIntValue* ToCUInt() { return nullptr; }
	[[nodiscard]] virtual CDoubleValue* ToCDouble() { return nullptr; }
	[[nodiscard]] virtual CStringValue* ToCString() { return nullptr; }
	[[nodiscard]] virtual CCallableValue* ToCallable() { return nullptr; }

	[[nodiscard]] virtual CArrayValue* ToArray() { return nullptr; }
	[[nodiscard]] virtual const CArrayValue* ToArray() const { return nullptr; }

	[[nodiscard]] virtual CObjectValue* ToObject() { return nullptr; }
	[[nodiscard]] virtual const CObjectValue* ToObject() const { return nullptr; }

	[[nodiscard]] constexpr auto GetAllocator() noexcept { return m_pAllocator; }
	[[nodiscard]] constexpr auto GetAllocator() const noexcept { return m_pAllocator; }

	[[nodiscard]] VarjusString Dump(VarjusUInt indent = 0, VarjusChar indentChar = '\t') const;
	[[nodiscard]] virtual constexpr std::size_t SizeOf() const noexcept { return std::size_t{ 0 }; }
	[[nodiscard]] virtual constexpr VarjusString ValueAsBytes() const noexcept { return VSL("\x00"); }

	[[nodiscard]] virtual std::size_t GetSharedPointer() const noexcept { return 0; }

protected:

	void ReleaseInternal();
	inline constexpr void ConstructInternal(CProgramRuntime* const allocator) { m_pAllocator = allocator; }

	void RemoveConstness() noexcept { m_bIsConst = false; }

	CVariable* m_pOwner{ nullptr };
	bool m_bIsConst{ false };
	CProgramRuntime* m_pAllocator{ nullptr };
};

template <typename Value>
class CValue : public IValue {
	static_assert(std::is_default_constructible_v<Value>, VSL("T must be default constructible"));
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
	//strings need it to be virtual
	[[nodiscard]] virtual constexpr std::size_t SizeOf() const noexcept override { return sizeof(Value); }
	[[nodiscard]] virtual constexpr VarjusString ValueAsBytes() const noexcept override { 
		return VarjusString((VarjusChar*)&Get(), SizeOf()); }

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

