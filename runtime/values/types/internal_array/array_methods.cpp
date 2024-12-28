#include "runtime/values/types/types.hpp"
#include "runtime/runtime.hpp"
#include "runtime/variables.hpp"
#include "runtime/exceptions/exception.hpp"
#include "runtime/values/simple_operators.hpp"
#include "runtime/structure.hpp"

#include "linter/context.hpp"

DECLARE_BUILT_IN_METHODS CArrayValue::m_oMethods;

FORWARD_DECLARE_METHOD(Push);
FORWARD_DECLARE_METHOD(PushFront);
FORWARD_DECLARE_METHOD(Pop);
FORWARD_DECLARE_METHOD(PopFront);
FORWARD_DECLARE_METHOD(Map);
FORWARD_DECLARE_METHOD(Find);
FORWARD_DECLARE_METHOD(FindLast);
FORWARD_DECLARE_METHOD(FindIndex);
FORWARD_DECLARE_METHOD(FindLastIndex);
FORWARD_DECLARE_METHOD(Filter);
FORWARD_DECLARE_METHOD(Contains);
FORWARD_DECLARE_METHOD(Reversed);
FORWARD_DECLARE_METHOD(Join);
FORWARD_DECLARE_METHOD(All);
FORWARD_DECLARE_METHOD(Any);

void CArrayValue::ConstructMethods()
{
	m_oMethods.clear();

	ADD_METHOD("push",            Push,          1u);
	ADD_METHOD("push_front",      PushFront,     1u);
	ADD_METHOD("pop",             Pop,           0u);
	ADD_METHOD("pop_front",       PopFront,      0u);
	ADD_METHOD("map",             Map,           1u);
	ADD_METHOD("find",            Find,          1u);
	ADD_METHOD("find_last",       FindLast,      1u);
	ADD_METHOD("find_index",      FindIndex,     1u);
	ADD_METHOD("find_last_index", FindLastIndex, 1u);
	ADD_METHOD("filter",          Filter,        1u);
	ADD_METHOD("contains",        Contains,      1u);
	ADD_METHOD("reversed",        Reversed,      0u);
	ADD_METHOD("join",            Join,          1u);
	ADD_METHOD("all",             All,           1u);
	ADD_METHOD("any",             Any,           1u);

}

#define START_METHOD(name) \
if(!_this)\
	throw CRuntimeError("attempted to call a method without \"this\" context"); \
if(_this->Type() != t_array) \
	throw CRuntimeError(std::format("array.{} expected an array, but got {}", #name, _this->TypeAsString()));\
auto name = _this->ToArray()

DEFINE_METHOD(Push)
{
	START_METHOD(__this);

	assert(newValues.size() == 1);
	auto& vars = __this->GetShared()->GetVariables();
	auto& newVar = vars.emplace_back(CProgramRuntime::AcquireNewVariable());
	newVar->SetValue(newValues.front()->Copy());
	return newVar->GetValue()->Copy();
}
DEFINE_METHOD(PushFront)
{
	START_METHOD(__this);
	auto& vars = __this->GetShared()->GetVariables();

	auto it = vars.insert(vars.begin(), CProgramRuntime::AcquireNewVariable());
	(*it)->SetValue(newValues.front()->Copy());
	return (*it)->GetValue()->Copy();
}

DEFINE_METHOD(Pop)
{

	START_METHOD(__this);
	auto& vars = __this->GetShared()->GetVariables();

	if (vars.empty())
		throw CRuntimeError("attempted to pop an empty array");

	auto& back = vars.back();

	auto copy = back->GetValue()->Copy();
	back->Release();
	vars.pop_back();

	return copy;
}
DEFINE_METHOD(PopFront)
{

	START_METHOD(__this);
	auto& vars = __this->GetShared()->GetVariables();

	if (vars.empty())
		throw CRuntimeError("attempted to pop_front an empty array");

	auto& front = vars.front();

	auto copy = front->GetValue()->Copy();
	front->Release();
	vars.erase(vars.begin());

	return copy;
}

DEFINE_METHOD(Map)
{
	assert(newValues.size() == 1);
	auto& mapFunc = newValues.front();

	if (!mapFunc->IsCallable())
		throw CRuntimeError(std::format("array.map expected \"callable\", but got \"{}\"", mapFunc->TypeAsString()));

	START_METHOD(__this);
	auto& vars = __this->GetShared()->GetVariables();

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

DEFINE_METHOD(Find) {
	START_METHOD(__this);
	return FindInternal(__this, ctx, newValues, true);
}
DEFINE_METHOD(FindLast) {
	START_METHOD(__this);
	return FindInternal(__this, ctx, newValues, false);
}

static inline IValue* FindTestValueIndex(CRuntimeContext* const ctx, IValue* const mapFunc, CVariable* const var, std::size_t i)
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
		result = CProgramRuntime::AcquireNewValue<CIntValue>(static_cast<std::int64_t>(i));
	}

	thisIteration->Release(); // nothing meaningful, release it
	return result;
}
static inline IValue* FindIndexInternal(CArrayValue* _this, CRuntimeContext* const ctx, const IValues& newValues, bool findFirst)
{
	assert(newValues.size() == 1);
	auto& mapFunc = newValues.front();

	if (!mapFunc->IsCallable())
		throw CRuntimeError(std::format("array.findindex expected \"callable\", but got \"{}\"", mapFunc->TypeAsString()));

	auto& vars = _this->GetShared()->GetVariables();

	IValue* result{ nullptr };
	IValues args(1);

	if (findFirst) {
		std::size_t i = 0u;

		for (const auto & var : vars) {
			result = FindTestValueIndex(ctx, mapFunc, var, i++);

			if (result)
				break;
		}
	} else {
		std::size_t i = vars.size();

		for (auto b = vars.rbegin(); b != vars.rend(); ++b) {
			result = FindTestValueIndex(ctx, mapFunc, *b, --i);

			if (result)
				break;
		}
	}

	if (CProgramRuntime::ExceptionThrown()) {
		return CProgramRuntime::GetExceptionValue();
	}

	if (!result)
		return CProgramRuntime::AcquireNewValue<CIntValue>(-1); //didn't find, return undefined

	return result;
}
DEFINE_METHOD(FindIndex) {
	START_METHOD(__this);
	return FindIndexInternal(__this, ctx, newValues, true);
}
DEFINE_METHOD(FindLastIndex) {
	START_METHOD(__this);
	return FindIndexInternal(__this, ctx, newValues, false);
}

DEFINE_METHOD(Filter) 
{
	assert(newValues.size() == 1);
	auto& mapFunc = newValues.front();

	if (!mapFunc->IsCallable())
		throw CRuntimeError(std::format("array.filter expected \"callable\", but got \"{}\"", mapFunc->TypeAsString()));

	START_METHOD(__this);
	auto& vars = __this->GetShared()->GetVariables();

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

DEFINE_METHOD(Contains) 
{
	assert(newValues.size() == 1);
	auto& searchElement = newValues.front();

	START_METHOD(__this);
	auto& vars = __this->GetShared()->GetVariables();

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
DEFINE_METHOD(Reversed) {
	
	START_METHOD(__this);
	auto& vars = __this->GetShared()->GetVariables();
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

DEFINE_METHOD(Join) 
{
	auto& delimiterValue = newValues.front();
	if (delimiterValue->Type() != t_string)
		throw CRuntimeError(std::format("array.join expected a string parameter, but got \"{}\"", delimiterValue->TypeAsString()));

	VectorOf<std::string> stringValues;
	START_METHOD(__this);
	auto& vars = __this->GetShared()->GetVariables();

	for (auto& var : vars) {
		auto& value = var->GetValue();
		if (value->Type() != t_string)
			throw CRuntimeError(std::format("array.join called on an array that contains \"{}\" instead of \"string\"", 
				delimiterValue->TypeAsString()));

		stringValues.push_back(value->ToString());
	}

	return CStringValue::Construct(JoinStrings(stringValues, delimiterValue->ToString()));
}

DEFINE_METHOD(All)
{
	assert(newValues.size() == 1);
	auto& mapFunc = newValues.front();

	if (!mapFunc->IsCallable())
		throw CRuntimeError(std::format("array.all expected \"callable\", but got \"{}\"", mapFunc->TypeAsString()));

	START_METHOD(__this);
	auto& vars = __this->GetShared()->GetVariables();

	IValues args(1);
	IValue* exception{ nullptr };

	bool all = true;

	//result array
	for (const auto& var : vars) {
		args[0] = var->GetValue()->Copy();

		IValue* thisIteration = mapFunc->Call(ctx, args);

		if (CProgramRuntime::ExceptionThrown()) {
			exception = thisIteration;
			break;
		}

		if (!thisIteration->IsBooleanConvertible())
			throw CRuntimeError(std::format("array.filter expected a boolean return value", mapFunc->TypeAsString()));

		if (!thisIteration->ToBoolean())
			all = false;

		thisIteration->Release(); // nothing meaningful, release it

		if (!all || CProgramRuntime::ExceptionThrown())
			break;
	}

	if (CProgramRuntime::ExceptionThrown()) {
		return exception;
	}

	return CProgramRuntime::AcquireNewValue<CBooleanValue>(all);
}
DEFINE_METHOD(Any)
{
	assert(newValues.size() == 1);
	auto& mapFunc = newValues.front();

	if (!mapFunc->IsCallable())
		throw CRuntimeError(std::format("array.all expected \"callable\", but got \"{}\"", mapFunc->TypeAsString()));

	START_METHOD(__this);
	auto& vars = __this->GetShared()->GetVariables();

	IValues args(1);
	IValue* exception{ nullptr };

	bool any = false;

	//result array
	for (const auto& var : vars) {
		args[0] = var->GetValue()->Copy();

		IValue* thisIteration = mapFunc->Call(ctx, args);

		if (CProgramRuntime::ExceptionThrown()) {
			exception = thisIteration;
			break;
		}

		if (!thisIteration->IsBooleanConvertible())
			throw CRuntimeError(std::format("array.filter expected a boolean return value", mapFunc->TypeAsString()));

		if (thisIteration->ToBoolean())
			any = true;

		thisIteration->Release(); // nothing meaningful, release it

		if (any || CProgramRuntime::ExceptionThrown())
			break;
	}

	if (CProgramRuntime::ExceptionThrown()) {
		return exception;
	}

	return CProgramRuntime::AcquireNewValue<CBooleanValue>(any);
}