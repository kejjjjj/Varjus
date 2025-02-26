#pragma once

#include "varjus_api/types/object.hpp"

#include <vector>
#include <memory>
#include <functional>
#include <optional>

struct BuiltInMethod_t;
struct BuiltInProperty_t;

class CBuiltInObject final : public CObjectValue
{
	NONCOPYABLE(CBuiltInObject);

public:
	CBuiltInObject();
	~CBuiltInObject();
	static CBuiltInObject* Construct(CProgramRuntime* const runtime, BuiltInMethod_t&& methods, BuiltInProperty_t&& properties);

	void Release() override;

	[[nodiscard]] IValue* Copy() override;
	[[nodiscard]] IValue* GetAggregate(std::size_t memberIdx) override;

private:
	std::shared_ptr<BuiltInMethod_t> m_oMethods;
	std::shared_ptr<BuiltInProperty_t> m_oProperties;
};

template<typename Type> requires std::is_same_v<BuiltInMethod_t, Type> || std::is_same_v<BuiltInProperty_t, Type>
using OptionalCtor = std::optional<std::function<Type(class CProgramInformation*)>>;

struct CBuiltInObjectPairs
{
	~CBuiltInObjectPairs();
	OptionalCtor<BuiltInMethod_t> methods;
	OptionalCtor<BuiltInProperty_t> properties;
};

class CBuiltInObjects {
public:
	void AddNewGlobalObject(const std::string& name, 
		const OptionalCtor<BuiltInMethod_t>& createMethods=std::nullopt,
		const OptionalCtor<BuiltInProperty_t>& createProperties=std::nullopt);

	[[nodiscard]] auto& Iterator() noexcept { return m_arrData; }
	void Reset() noexcept { m_arrData.clear(); }
private:
	std::vector<std::pair<std::string, CBuiltInObjectPairs>> m_arrData;

};
