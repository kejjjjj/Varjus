#pragma once

#include "api/types/object.hpp"
#include "api/types/internal/methods.hpp"

#include <vector>
#include <memory>
#include <functional>

class CBuiltInObject final : public CObjectValue
{
	NONCOPYABLE(CBuiltInObject);

public:
	CBuiltInObject();
	~CBuiltInObject();
	static CBuiltInObject* Construct(BuiltInMethod_t&& methods);

	void Release() override;

	[[nodiscard]] IValue* Copy() override;
	[[nodiscard]] IValue* GetAggregate(std::size_t memberIdx) override;

private:
	std::shared_ptr<BuiltInMethod_t> m_oMethods;
};

class CBuiltInObjects {
public:
	static void AddNewGlobalObject(const std::string& name, const std::function<BuiltInMethod_t()>& createMethods);

	[[nodiscard]] static auto& Iterator() noexcept { return m_arrMethods; }
	static void Reset() noexcept { m_arrMethods.clear(); }
private:
	static std::vector<std::pair<std::string, std::function<BuiltInMethod_t()>>> m_arrMethods;
};
