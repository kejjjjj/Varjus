#pragma once

#include "varjus_api/types/object.hpp"

#include <vector>
#include <memory>
#include <functional>
#include <optional>

struct BuiltInMethod_t;
struct BuiltInProperty_t;
struct ObjectDeclaration_t;

class CBuiltInObject final : public CObjectValue
{
	NONCOPYABLE(CBuiltInObject);

public:
	CBuiltInObject();
	~CBuiltInObject();
	static CBuiltInObject* Construct(CProgramRuntime* const runtime, BuiltInMethod_t&& methods, BuiltInProperty_t&& properties);

	void Release() override;

	[[nodiscard]] virtual const constexpr CBuiltInObject* ToBuiltInObject() const noexcept { return this; }
	[[nodiscard]] virtual constexpr CBuiltInObject* ToBuiltInObject() noexcept { return this; }

	[[nodiscard]] IValue* Copy() override;
	[[nodiscard]] IValue* Index(CRuntimeContext* const ctx, IValue* index) override;
	[[nodiscard]] IValue* GetAggregate(CRuntimeContext* const ctx, std::size_t memberIdx) override;

private:
	[[nodiscard]] VarjusString ValueAsString() const override;
	[[nodiscard]] VarjusString ValueAsEscapedString() const override { return CBuiltInObject::ValueAsString(); }

	std::shared_ptr<BuiltInMethod_t> m_oMethods;
	std::shared_ptr<BuiltInProperty_t> m_oProperties;

	friend VarjusString DumpBuiltInObject(VarjusUInt indent, VarjusChar indentChar, const CBuiltInObject* obj);
};

template<typename Type> requires std::is_same_v<void, std::remove_reference_t<Type>>
using OptionalCtor = std::optional<std::function<Type(ObjectDeclaration_t&)>>;

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
