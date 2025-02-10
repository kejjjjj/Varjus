#pragma once

#include "runtime/values/types/object.hpp"
#include "runtime/values/types/internal/methods.hpp"

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