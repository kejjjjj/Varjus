#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>

#include "varjus_api/internal/globalDefinitions.hpp"

class IValue;
class CProgramRuntime;

class CVariable
{
	NONCOPYABLE(CVariable);
	friend class CProgramRuntime;
public:
	CVariable() = default;
	virtual ~CVariable() = default;

	[[nodiscard]] static CVariable* Construct(CProgramRuntime* const runtime);
	[[nodiscard]] static CVariable* Construct(CProgramRuntime* const runtime, IValue* v);

	void SetValue(IValue* v);
	[[maybe_unused]] virtual bool Release();

	[[nodiscard]] inline auto& GetValue() noexcept { return m_pValue; }
	[[nodiscard]] virtual constexpr CVariable* Copy() noexcept { m_uRefCount++; return this; }
	[[nodiscard]] constexpr std::size_t& RefCount() noexcept { return m_uRefCount; }

	[[nodiscard]] constexpr virtual bool IsChild() const noexcept { return false; }
	[[nodiscard]] constexpr virtual IValue* GetParent() const noexcept { return nullptr; }

	bool m_bSelfCapturing{ false }; //not exactly the most efficient way to include this here

protected:
	IValue* m_pValue{};
	std::size_t m_uRefCount{std::size_t(1)};
	CProgramRuntime* m_pAllocator{ nullptr };
};

//for array members and object properties
class CChildVariable final : public CVariable
{
	NONCOPYABLE(CChildVariable);

public:

	CChildVariable() = default;
	~CChildVariable() = default;

	[[maybe_unused]] bool Release() override;


	[[nodiscard]] static CChildVariable* Construct(CProgramRuntime* const runtime, IValue* parent);
	[[nodiscard]] static CChildVariable* Construct(CProgramRuntime* const runtime, IValue* v, IValue* parent);

	[[nodiscard]] constexpr CVariable* Copy() noexcept override { m_uRefCount++; return this; }

	[[nodiscard]] constexpr virtual bool IsChild() const noexcept { return true; }
	[[nodiscard]] constexpr IValue* GetParent() const noexcept override { return m_pParent; }

	constexpr void SetParent(IValue* parent) { m_pParent = parent; }
private:
	IValue* m_pParent{};
};
