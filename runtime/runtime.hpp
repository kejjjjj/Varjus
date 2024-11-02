#pragma once

#include <string>

#include "globalDefinitions.hpp"
#include "pools/object_pool_non_owning.hpp"
#include "pools/object_pool_owning.hpp"

class IValue;
class CBooleanValue;
class CIntValue;
class CDoubleValue;
class CStringValue;

class CVariable;
class CRuntimeFunction;
class CFileRuntimeData;

using RuntimeFunction = std::unique_ptr<CRuntimeFunction>;


class CProgramRuntime
{
	NONCOPYABLE(CProgramRuntime);

	friend class CRuntimeFunction;
	friend class CRuntimeExpression;

public:
	// only one file for now
	CProgramRuntime(CFileRuntimeData* const file);
	~CProgramRuntime();

	void Execute();

	static IValue* AcquireNewValue();
	static CBooleanValue* AcquireNewBooleanValue(bool value);
	static CIntValue* AcquireNewIntValue(std::int64_t value);
	static CDoubleValue* AcquireNewDoubleValue(double value);
	static CStringValue* AcquireNewStringValue(const std::string& value);

	static void FreeUndefinedValue(IValue* value);
	static void FreeBooleanValue(CBooleanValue* value);
	static void FreeIntValue(CIntValue* value);
	static void FreeDoubleValue(CDoubleValue* value);
	static void FreeStringValue(CStringValue* value);

private:
	std::vector<RuntimeFunction> m_oFunctions;
	static COwningObjectPool<IValue> m_oUndefinedValuePool;
	static COwningObjectPool<CBooleanValue> m_oBooleanValuePool;
	static COwningObjectPool<CIntValue> m_oIntValuePool;
	static COwningObjectPool<CDoubleValue> m_oDoubleValuePool;
	static COwningObjectPool<CStringValue> m_oStringValuePool;

	static CNonOwningObjectPool<CVariable> m_oVariablePool;
};
