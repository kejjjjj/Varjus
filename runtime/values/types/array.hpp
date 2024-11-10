#pragma once

#include "internal/shared_ownership.hpp"
#include <vector>

template<typename T>
using VectorOf = std::vector<T>;

class CArrayValue : public CValue<std::shared_ptr<CInternalArrayValue>>
{
public:
	CArrayValue() = default;
	CArrayValue(VectorOf<IValue*>&& v);

	//copy constructor
	[[nodiscard]] EValueType Type() const noexcept override { return t_array; };

	void CreateOwnership();
	void Release() override;

	[[nodiscard]] constexpr bool IsIndexable() const noexcept override { return true; }

	[[nodiscard]] CArrayValue* MakeShared() const;
	[[nodiscard]] IValue* Copy() override;
	[[nodiscard]] CArrayValue* ToArray() override;
	[[nodiscard]] CInternalArrayValue* Internal();
	[[nodiscard]] CInternalArrayValue* Internal() const;
	
	[[nodiscard]] IValue* Index(std::int64_t index) override;

private:
	[[nodiscard]] std::string TypeAsString() const override { return "array"s; }
	[[nodiscard]] std::string ValueAsString() const override;

};

class CInternalArrayValue : public CValue<VectorOf<std::unique_ptr<CVariable>>>
{
public:
	CInternalArrayValue() = default;
	CInternalArrayValue(VectorOf<IValue*>&& v);

	void Release() override;

	void Set(VectorOf<IValue*>&& v);
};
