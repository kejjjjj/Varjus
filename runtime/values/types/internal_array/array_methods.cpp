#include "runtime/values/types/types.hpp"
#include "runtime/runtime.hpp"
#include "runtime/variables.hpp"
#include "runtime/exceptions/exception.hpp"
#include "runtime/values/simple_operators.hpp"

CArrayValue::ArrayMethods CArrayValue::ConstructMethods()
{
	return {
		{"push",       {1u, &CArrayValue::Push}},
		{"push_front", {1u, &CArrayValue::PushFront}},
		{"pop",        {0u, &CArrayValue::Pop}},
		{"pop_front",  {0u, &CArrayValue::PopFront}},
		{"map",        {1u, &CArrayValue::Map}},
		{"find",       {1u, &CArrayValue::Find}},
		{"find_last",  {1u, &CArrayValue::FindLast}},
		{"filter",     {1u, &CArrayValue::Filter}},
		{"contains",   {1u, &CArrayValue::Contains}},
		{"reverse",    {0u, &CArrayValue::Reverse}},
		{"join",	   {1u, &CArrayValue::Join}},

	};
}

IValue* CArrayValue::Push([[maybe_unused]] CRuntimeContext* const ctx, const IValues& newValues)
{
	assert(newValues.size() == 1);
	auto& vars = GetShared()->GetVariables();
	auto& newVar = vars.emplace_back(CProgramRuntime::AcquireNewVariable());
	newVar->SetValue(newValues.front()->Copy());
	return newVar->GetValue()->Copy();
}
IValue* CArrayValue::PushFront([[maybe_unused]] CRuntimeContext* const ctx, const IValues& newValues)
{
	assert(newValues.size() == 1);
	auto& vars = GetShared()->GetVariables();

	auto it = vars.insert(vars.begin(), CProgramRuntime::AcquireNewVariable());
	(*it)->SetValue(newValues.front()->Copy());
	return (*it)->GetValue()->Copy();
}

IValue* CArrayValue::Pop([[maybe_unused]] CRuntimeContext* const ctx, [[maybe_unused]] const IValues& newValues)
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
IValue* CArrayValue::PopFront([[maybe_unused]] CRuntimeContext* const ctx, [[maybe_unused]] const IValues& newValues)
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

IValue* CArrayValue::Map(CRuntimeContext* const ctx, const IValues& newValues)
{
	assert(newValues.size() == 1);
	auto& mapFunc = newValues.front();

	if (!mapFunc->IsCallable())
		throw CRuntimeError(std::format("array.map expected \"callable\", but got \"{}\"", mapFunc->TypeAsString()));

	auto& vars = GetShared()->GetVariables();

	IValues results(vars.size());
	IValues args(1);
	IValue* exceptionValue{ nullptr };

	//result array
	for (auto i = std::size_t(0); const auto& var : vars) {
		args[0] = var->GetValue()->Copy();
		results[i++] = mapFunc->Call(ctx, args);

		//because of course someone will throw an exception :x
		if (CProgramRuntime::ExceptionThrown()) {
			exceptionValue = results[i - 1];
			results.resize(i - 1);
			break;
		}

	}

	if (CProgramRuntime::ExceptionThrown()) {

		for (auto& r : results) {
			if(r != exceptionValue)
				r->Release();
		}

		return exceptionValue;
	}

	
	return CArrayValue::Construct(std::move(results));
}
static inline IValue* FindTestValue(CRuntimeContext* const ctx, IValue* const mapFunc, CVariable* const var)
{
	IValues args(1);
	args[0] = var->GetValue()->Copy();

	IValue* thisIteration = mapFunc->Call(ctx, args);
	IValue* result{ nullptr };

	if (CProgramRuntime::ExceptionThrown()) {
		return CProgramRuntime::GetExceptionValue();
	}

	if (!thisIteration->IsBooleanConvertible())
		throw CRuntimeError(std::format("array.find expected a boolean return value", mapFunc->TypeAsString()));

	if (thisIteration->ToBoolean()) {
		result = var->GetValue();
	}

	thisIteration->Release(); // nothing meaningful, release it
	return result;
}
static inline IValue* FindInternal(CArrayValue* _this, CRuntimeContext* const ctx, const IValues& newValues, bool findFirst)
{
	assert(newValues.size() == 1);
	auto& mapFunc = newValues.front();

	if (!mapFunc->IsCallable())
		throw CRuntimeError(std::format("array.find expected \"callable\", but got \"{}\"", mapFunc->TypeAsString()));

	auto& vars = _this->GetShared()->GetVariables();

	IValue* result{ nullptr };

	IValues args(1);

	if (findFirst) {
		for (const auto& var : vars) {
			result = FindTestValue(ctx, mapFunc, var);

			if (result)
				break;
		}
	} else {
		for (auto b = vars.rbegin(); b != vars.rend(); ++b) {
			result = FindTestValue(ctx, mapFunc, *b);

			if (result)
				break;
		}
	}

	if (CProgramRuntime::ExceptionThrown()) {
		return CProgramRuntime::GetExceptionValue();
	}

	if (!result)
		return CProgramRuntime::AcquireNewValue<IValue>(); //didn't find, return undefined

	return result->Copy();
}
IValue* CArrayValue::Find(CRuntimeContext* const ctx, const IValues& newValues)
{
	return FindInternal(this, ctx, newValues, true);
}
IValue* CArrayValue::FindLast(CRuntimeContext* const ctx, const IValues& newValues)
{
	return FindInternal(this, ctx, newValues, false);
}
IValue* CArrayValue::Filter(CRuntimeContext* const ctx, const IValues& newValues)
{
	assert(newValues.size() == 1);
	auto& mapFunc = newValues.front();

	if (!mapFunc->IsCallable())
		throw CRuntimeError(std::format("array.filter expected \"callable\", but got \"{}\"", mapFunc->TypeAsString()));

	auto& vars = GetShared()->GetVariables();

	IValues results;
	IValues args(1);
	IValue* exception{ nullptr };

	//result array
	for (const auto & var : vars) {
		args[0] = var->GetValue()->Copy();

		IValue* thisIteration = mapFunc->Call(ctx, args);

		if (CProgramRuntime::ExceptionThrown()) {
			exception = thisIteration;
			break;
		}

		if (!thisIteration->IsBooleanConvertible())
			throw CRuntimeError(std::format("array.filter expected a boolean return value", mapFunc->TypeAsString()));

		if (thisIteration->ToBoolean())
			results.push_back(var->GetValue()->Copy());

		thisIteration->Release(); // nothing meaningful, release it

		if (CProgramRuntime::ExceptionThrown())
			break;
	}

	if (CProgramRuntime::ExceptionThrown()) {
		return exception;
	}

	return CArrayValue::Construct(std::move(results));
}

IValue* CArrayValue::Contains([[maybe_unused]] CRuntimeContext* const ctx, const IValues& newValues)
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

	if (!result)
		return CProgramRuntime::AcquireNewValue<CBooleanValue>(false);

	return result;
}
IValue* CArrayValue::Reverse([[maybe_unused]] CRuntimeContext* const ctx, [[maybe_unused]] const IValues& newValues)
{
	
	auto& vars = GetShared()->GetVariables();
	IValues valuesAsCopy(vars.size());

	for (auto i = size_t(0); auto& var : vars)
		valuesAsCopy[i++] = var->GetValue()->Copy();
	
	std::reverse(valuesAsCopy.begin(), valuesAsCopy.end());

	return CArrayValue::Construct(std::move(valuesAsCopy));
}

std::string JoinStrings(const VectorOf<std::string>& strings, const std::string& delimiter) {
	std::ostringstream result;
	for (auto i = std::size_t(0); i < strings.size(); ++i) {
		result << strings[i];
		if (i != strings.size() - std::size_t(1)) {
			result << delimiter;
		}
	}
	return result.str();
}

IValue* CArrayValue::Join([[maybe_unused]] CRuntimeContext* const ctx, [[maybe_unused]] const IValues& newValues)
{
	auto& delimiterValue = newValues.front();
	if (delimiterValue->Type() != t_string)
		throw CRuntimeError(std::format("array.join expected a string parameter, but got \"{}\"", delimiterValue->TypeAsString()));

	VectorOf<std::string> stringValues;
	auto& vars = GetShared()->GetVariables();



	for (auto& var : vars) {
		auto& value = var->GetValue();
		if (value->Type() != t_string)
			throw CRuntimeError(std::format("array.join called on an array that contains \"{}\" instead of \"string\"", 
				delimiterValue->TypeAsString()));

		stringValues.push_back(value->ToString());
	}

	return CStringValue::Construct(JoinStrings(stringValues, delimiterValue->ToString()));
}