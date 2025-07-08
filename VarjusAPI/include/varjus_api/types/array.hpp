#pragma once

#include "default.hpp"
#include "internal/aggregate.hpp"

#include <vector>
#include <memory>


class CInternalArrayValue;
class CChildVariable;
class CArrayValue;

using ArrayOwner = std::weak_ptr<CInternalArrayValue>;

struct CArrayContent final
{
	[[maybe_unused]] CChildVariable* PushVariable(Varjus::CProgramRuntime* const runtime, IValue* value);
	[[maybe_unused]] CChildVariable* PushFrontVariable(Varjus::CProgramRuntime* const runtime, IValue* value);
	
	constexpr auto& GetVariables() noexcept { return m_oVariables; }
	constexpr auto& GetVariables() const noexcept { return m_oVariables; }

	//more persistent than a simple reference to a CArrayValue
	ArrayOwner m_pArrayOwner;
private:
	VectorOf<CChildVariable*> m_oVariables;
};

class CInternalArrayValue final
{
public:
	CInternalArrayValue() = default;
	~CInternalArrayValue();

	void Release();

	void Set(const ArrayOwner& self, Varjus::CProgramRuntime* const runtime, VectorOf<IValue*>&& v);
	constexpr auto& Get() noexcept { return m_oValue; }
	constexpr auto& Get() const noexcept { return m_oValue; }

	[[nodiscard]] constexpr auto& GetContent() noexcept { return Get(); }
	[[nodiscard]] constexpr auto& GetContent() const noexcept { return Get(); }

	[[nodiscard]] std::size_t Length() const noexcept;

protected:
	CArrayContent m_oValue;
};

namespace Varjus {
	struct BuiltInMethod_t; 
	struct BuiltInProperty_t;
	class CProgramInformation;
}
class CArrayValue final : public CValue<CInternalArrayValue>
{
public:
	CArrayValue() = default;
	~CArrayValue();
	
	static CArrayValue* Construct(Varjus::CProgramRuntime* const runtime, IValues&& values);
	[[nodiscard]] static std::unique_ptr<Varjus::BuiltInMethod_t> ConstructMethods(Varjus::CProgramInformation* const info);
	[[nodiscard]] static std::unique_ptr<Varjus::BuiltInProperty_t> ConstructProperties(Varjus::CProgramInformation* const info);

	[[nodiscard]] Varjus::EValueType Type() const noexcept override { return Varjus::t_array; };

	void Release() override;

	[[nodiscard]] constexpr bool IsIndexable() const noexcept override { return true; }
	[[nodiscard]] constexpr bool IsAggregate() const noexcept override { return true; }
	[[nodiscard]] constexpr bool IsBooleanConvertible() const noexcept override { return false; }
	[[nodiscard]] constexpr bool IsIterable() const noexcept override { return true; }

	[[nodiscard]] IValue* Copy() override;
	[[nodiscard]] CArrayValue* ToArray() override { return this; }
	[[nodiscard]] const CArrayValue* ToArray() const override { return this; }

	[[nodiscard]] CInternalArrayValue* Internal();
	[[nodiscard]] CInternalArrayValue* Internal() const;
	
	[[nodiscard]] IValue* Index(CRuntimeContext* const ctx, IValue* index) override;
	[[nodiscard]] IValue* GetAggregate(CRuntimeContext* const ctx, std::size_t memberIdx) override;
	[[nodiscard]] IValues ToIterable() const override;

	[[nodiscard]] std::size_t AddressOf() const noexcept override { 
		return reinterpret_cast<std::size_t>(GetShared().get()); 
	}

	[[maybe_unused]] CChildVariable* PushVariable(Varjus::CProgramRuntime* const runtime, IValue* value);
	[[maybe_unused]] CChildVariable* PushFrontVariable(Varjus::CProgramRuntime* const runtime, IValue* value);

	[[nodiscard]] std::size_t GetSharedPointer() const noexcept override { return reinterpret_cast<std::size_t>(GetShared().get()); }
private:
	[[nodiscard]] VarjusString TypeAsString() const override { return VSL("array"); }
	[[nodiscard]] VarjusString ValueAsString() const override;
	[[nodiscard]] VarjusString ValueAsEscapedString() const override { return ValueAsString(); }
};


