#pragma once

#ifdef OPTIMIZATIONS

#include "varjus_api/internal/globalDefinitions.hpp"



#include <cstdint>
#include <vector>
#include "linter/expressions/definitions.hpp"

using namespace std::string_literals;

/***********************************************************************
 > Represents values which can be evaluated during linting
***********************************************************************/

inline VarjusString opt_emptyString;

class CConstEvalVariable;
class ConstantASTNode;

class CConstEvalBooleanValue;
class CConstEvalIntValue;
class CConstEvalDoubleValue;
class CConstEvalStringValue;

class IConstEvalValue
{
public:
	virtual ~IConstEvalValue() = default;

	[[nodiscard]] constexpr virtual EValueType Type() const noexcept { return t_undefined; }

	[[nodiscard]] virtual VarjusString TypeAsString() const { return VSL("undefined"); }
	[[nodiscard]] virtual VarjusString ValueAsString() const { return VSL("undefined"); }

	constexpr void SetPosition(const CodePosition& pos) noexcept { m_oApproximatePosition = pos; }
	virtual void Release();
	[[nodiscard]] virtual IConstEvalValue* Copy();

	[[nodiscard]] virtual std::shared_ptr<ConstantASTNode> ToAST() const;


	constexpr auto SetOwner(CConstEvalVariable* o) noexcept { m_pOwner = o; }
	[[nodiscard]] constexpr bool HasOwner() const noexcept { return !!m_pOwner; }
	[[nodiscard]] constexpr auto GetOwner() const noexcept { return m_pOwner; }

	[[nodiscard]] constexpr virtual bool IsCoerceable() const noexcept { return false; }

	[[nodiscard]] bool& AsBoolean();
	[[nodiscard]] VarjusInt& AsInt();
	[[nodiscard]] VarjusDouble& AsDouble();
	[[nodiscard]] VarjusString& AsString();

	[[nodiscard]] virtual bool ToBoolean() const { return false; }
	[[nodiscard]] virtual VarjusInt ToInt() const { return 0; }
	[[nodiscard]] virtual VarjusDouble ToDouble() const { return 0.0; }
	[[nodiscard]] virtual const VarjusString& ToString() const { return opt_emptyString; }

	[[nodiscard]] virtual CConstEvalBooleanValue* ToCBoolean() { return nullptr; }
	[[nodiscard]] virtual CConstEvalIntValue* ToCInt() { return nullptr; }
	[[nodiscard]] virtual CConstEvalDoubleValue* ToCDouble() { return nullptr; }
	[[nodiscard]] virtual CConstEvalStringValue* ToCString() { return nullptr; }

protected:
	void ReleaseInternal();

	CConstEvalVariable* m_pOwner{ nullptr };
	CodePosition m_oApproximatePosition;
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
	Value m_oValue{};
};

#endif