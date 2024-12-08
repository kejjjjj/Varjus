#include "runtime/values/types/types.hpp"
#include "runtime/runtime.hpp"
#include "runtime/variables.hpp"
#include "runtime/exceptions/exception.hpp"
#include "runtime/values/simple_operators.hpp"

IValue* CArrayValue::Push([[maybe_unused]] CFunction* const thisFunction, const IValues& newValues)
{
	assert(newValues.size() == 1);
	auto& vars = GetShared()->GetVariables();
	auto& newVar = vars.emplace_back(CProgramRuntime::AcquireNewVariable());
	newVar->SetValue(newValues.front());
	return newVar->GetValue()->Copy();
}
IValue* CArrayValue::PushFront([[maybe_unused]] CFunction* const thisFunction, const IValues& newValues)
{
	assert(newValues.size() == 1);
	auto& vars = GetShared()->GetVariables();

	auto it = vars.insert(vars.begin(), CProgramRuntime::AcquireNewVariable());
	(*it)->SetValue(newValues.front());
	return (*it)->GetValue()->Copy();
}

IValue* CArrayValue::Pop([[maybe_unused]] CFunction* const thisFunction, [[maybe_unused]] const IValues& newValues)
{

	auto& vars = GetShared()->GetVariables();

	if (vars.empty())
		throw CRuntimeError("attempted to pop an empty array");

	auto& back = vars.back();

	auto copy = back->GetValue()->Copy();
	back->Release();
	vars.pop_back();

	return copy;
}
IValue* CArrayValue::PopFront([[maybe_unused]] CFunction* const thisFunction, [[maybe_unused]] const IValues& newValues)
{

	auto& vars = GetShared()->GetVariables();

	if (vars.empty())
		throw CRuntimeError("attempted to pop_front an empty array");

	auto& front = vars.front();

	auto copy = front->GetValue()->Copy();
	front->Release();
	vars.erase(vars.begin());

	return copy;
}

IValue* CArrayValue::Map(CFunction* const thisFunction, const IValues& newValues)
{
	assert(newValues.size() == 1);
	auto& mapFunc = newValues.front();

	if (!mapFunc->IsCallable())
		throw CRuntimeError(std::format("array.map expected \"callable\", but got \"{}\"", mapFunc->TypeAsString()));

	auto& vars = GetShared()->GetVariables();

	IValues results(vars.size());
	IValues args(1);

	//result array
	for (auto i = std::size_t(0); const auto& var : vars) {
		args[0] = var->GetValue()->Copy();
		results[i++] = mapFunc->Call(thisFunction, args);
	}

	if (mapFunc->IsHanging())
		mapFunc->Release();

	return CArrayValue::Construct(std::move(results));
}
IValue* CArrayValue::Find(CFunction* const thisFunction, const IValues& newValues)
{
	assert(newValues.size() == 1);
	auto& mapFunc = newValues.front();

	if (!mapFunc->IsCallable())
		throw CRuntimeError(std::format("array.find expected \"callable\", but got \"{}\"", mapFunc->TypeAsString()));

	auto& vars = GetShared()->GetVariables();

	IValue* result{ nullptr };
	IValues args(1);

	for (const auto& var : vars) {
		args[0] = var->GetValue()->Copy();
		IValue* thisIteration = mapFunc->Call(thisFunction, args);

		if(!thisIteration->IsBooleanConvertible())
			throw CRuntimeError(std::format("array.find expected a boolean return value", mapFunc->TypeAsString()));

		if (thisIteration->ToBoolean()) {
			result = var->GetValue();
		}

		thisIteration->Release(); // nothing meaningful, release it

		if (result)
			break;
	}

	if (mapFunc->IsHanging())
		mapFunc->Release();

	if (!result)
		return CProgramRuntime::AcquireNewValue<IValue>(); //didn't find, return undefined

	return result->Copy();
}
IValue* CArrayValue::Filter(CFunction* const thisFunction, const IValues& newValues)
{
	assert(newValues.size() == 1);
	auto& mapFunc = newValues.front();

	if (!mapFunc->IsCallable())
		throw CRuntimeError(std::format("array.filter expected \"callable\", but got \"{}\"", mapFunc->TypeAsString()));

	auto& vars = GetShared()->GetVariables();

	IValues results;
	IValues args(1);

	//result array
	for (const auto & var : vars) {
		args[0] = var->GetValue()->Copy();

		IValue* thisIteration = mapFunc->Call(thisFunction, args);

		if (!thisIteration->IsBooleanConvertible())
			throw CRuntimeError(std::format("array.filter expected a boolean return value", mapFunc->TypeAsString()));

		if (thisIteration->ToBoolean())
			results.push_back(var->GetValue()->Copy());

		thisIteration->Release(); // nothing meaningful, release it
	}

	if (mapFunc->IsHanging())
		mapFunc->Release();

	return CArrayValue::Construct(std::move(results));
}

IValue* CArrayValue::Contains([[maybe_unused]] CFunction* const thisFunction, const IValues& newValues)
{
	assert(newValues.size() == 1);
	auto& searchElement = newValues.front();

	auto& vars = GetShared()->GetVariables();

	IValue* result{ nullptr };
	IValues args(1);

	for (const auto& var : vars) {
		IValue* thisIteration = OP_STRICT_EQUALITY(var->GetValue(), searchElement);

		if (thisIteration->ToBoolean()) {
			result = thisIteration;
			break;
		}

		thisIteration->Release(); // nothing meaningful, release it
	}

	if (!searchElement->HasOwner())
		searchElement->Release();

	if (!result)
		return CProgramRuntime::AcquireNewValue<CBooleanValue>(false);

	return result;
}