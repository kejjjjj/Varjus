#pragma once

#include "globalDefinitions.hpp"


#include <string>
#include <cstdint>
#include <vector>

template<typename T>
using VectorOf = std::vector<T>;

using namespace std::string_literals;

/***********************************************************************
 > Represents values which can be evaluated during linting
***********************************************************************/

struct CLinterVariable;

class IConstEvalValue
{
public:
	virtual ~IConstEvalValue() = default;

	[[nodiscard]] constexpr virtual EValueType Type() const noexcept { return t_undefined; }
	[[nodiscard]] virtual std::int64_t ToInt() const { return 0; }

	[[nodiscard]] virtual std::string TypeAsString() const { return "undefined"s; }
	[[nodiscard]] virtual std::string ValueAsString() const { return "undefined"s; }

	virtual void Release();
	[[nodiscard]] virtual IConstEvalValue* Copy();

	constexpr auto SetOwner(CLinterVariable* o) noexcept { m_pOwner = o; }
	[[nodiscard]] constexpr bool HasOwner() const noexcept { return !!m_pOwner; }
	[[nodiscard]] constexpr auto& GetOwner() const noexcept { return m_pOwner; }

	[[nodiscard]] constexpr virtual bool IsCoerceable() const noexcept { return false; }

protected:
	void ReleaseInternal();

	CLinterVariable* m_pOwner{ nullptr };
};

template <typename Value>
class CConstEvalValue : public IConstEvalValue
{
public:
	CConstEvalValue() = default;
	virtual ~CConstEvalValue() = default;

	[[nodiscard]] constexpr Value& Get() noexcept { return m_oValue; }
	[[nodiscard]] constexpr const Value& Get() const noexcept { return m_oValue; }

	void SetStorageValue(Value&& v) { m_oValue = std::move(v); }
	void SetStorageValue(const Value& v) { m_oValue = v; }

protected:
	Value m_oValue;
};

