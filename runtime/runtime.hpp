#pragma once

#include "globalDefinitions.hpp"
#include "pools/object_pool_non_owning.hpp"
#include "pools/object_pool_owning.hpp"

class IValue;
class CIntValue;
class CDoubleValue;

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
	static CIntValue* AcquireNewIntValue();
	static CDoubleValue* AcquireNewDoubleValue();

	static void FreeUndefinedValue(IValue* value);
	static void FreeIntValue(CIntValue* value);
	static void FreeDoubleValue(CDoubleValue* value);

private:
	std::vector<RuntimeFunction> m_oFunctions;
	static COwningObjectPool<IValue> m_oUndefinedValuePool;
	static COwningObjectPool<CIntValue> m_oIntValuePool;
	static COwningObjectPool<CDoubleValue> m_oDoubleValuePool;

	static CNonOwningObjectPool<CVariable> m_oVariablePool;
};
