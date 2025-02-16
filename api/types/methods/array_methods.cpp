#include "api/types/types.hpp"
#include "api/types/operators/default_operators.hpp"

#include "runtime/runtime.hpp"
#include "runtime/variables.hpp"
#include "runtime/exceptions/exception.hpp"
#include "runtime/structure.hpp"

#include "linter/context.hpp"

#include <algorithm>

BuiltInMethod_t CArrayValue::m_oMethods;
BuiltInProperty_t CArrayValue::m_oProperties;

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
FORWARD_DECLARE_METHOD(Slice);
FORWARD_DECLARE_METHOD(Sort);
FORWARD_DECLARE_METHOD(Resize);
FORWARD_DECLARE_METHOD(Fill);

void CArrayValue::ConstructMethods()
{
	m_oMethods.clear();

	m_oMethods.AddMethod("push",            Push,          1u);
	m_oMethods.AddMethod("push_front",      PushFront,     1u);
	m_oMethods.AddMethod("pop",             Pop,           0u);
	m_oMethods.AddMethod("pop_front",       PopFront,      0u);
	m_oMethods.AddMethod("map",             Map,           1u);
	m_oMethods.AddMethod("find",            Find,          1u);
	m_oMethods.AddMethod("find_last",       FindLast,      1u);
	m_oMethods.AddMethod("find_index",      FindIndex,     1u);
	m_oMethods.AddMethod("find_last_index", FindLastIndex, 1u);
	m_oMethods.AddMethod("filter",          Filter,        1u);
	m_oMethods.AddMethod("contains",        Contains,      1u);
	m_oMethods.AddMethod("reversed",        Reversed,      0u);
	m_oMethods.AddMethod("join",            Join,          1u);
	m_oMethods.AddMethod("all",             All,           1u);
	m_oMethods.AddMethod("any",             Any,           1u);
	m_oMethods.AddMethod("slice",	        Slice,         2u);
	m_oMethods.AddMethod("sort",            Sort,          1u);
	m_oMethods.AddMethod("resize",          Resize,        1u);
	m_oMethods.AddMethod("fill",            Fill,          1u);

}

FORWARD_DECLARE_PROPERTY(ArrayLength);

void CArrayValue::ConstructProperties()
{
	m_oProperties.clear();

	m_oProperties.AddProperty("length", ArrayLength);
}



#define START_METHOD(name) \
if(!_this)\
	throw CRuntimeError("attempted to call a method without \"this\" context"); \
if(_this->Type() != t_array) \
	throw CRuntimeError(std::format("array.{} expected an array, but got {}", #name, _this->TypeAsString()));\
auto name = _this->ToArray()

DEFINE_PROPERTY(ArrayLength) {
	START_METHOD(__this);
	return CProgramRuntime::AcquireNewValue<CIntValue>(static_cast<std::int64_t>(__this->Internal()->Length()));
}

DEFINE_METHOD(Push)
{
	START_METHOD(__this);

	assert(args.size() == 1);
	auto& vars = __this->GetShared()->GetVariables();
	auto& newVar = vars.emplace_back(CProgramRuntime::AcquireNewVariable());
	newVar->SetValue(args.front()->Copy());
	return newVar->GetValue()->Copy();
}
DEFINE_METHOD(PushFront)
{
	START_METHOD(__this);
	auto& vars = __this->GetShared()->GetVariables();

	auto it = vars.insert(vars.begin(), CProgramRuntime::AcquireNewVariable());
	(*it)->SetValue(args.front()->Copy());
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
	assert(args.size() == 1);
	auto& mapFunc = args.front();

	if (!mapFunc->IsCallable())
		throw CRuntimeError(std::format("array.map expected \"callable\", but got \"{}\"", mapFunc->TypeAsString()));

	START_METHOD(__this);
	auto& vars = __this->GetShared()->GetVariables();

	IValues results(vars.size());
	IValues call_args(1);
	IValue* exceptionValue{ nullptr };

	//result array
	for (auto i = std::size_t(0); const auto& var : vars) {
		call_args[0] = var->GetValue()->Copy();
		results[i++] = mapFunc->Call(ctx, call_args);

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
static inline IValue* FindInternal(CArrayValue* _this, CRuntimeContext* const ctx, const IValues& args, bool findFirst)
{
	assert(args.size() == 1);
	auto& mapFunc = args.front();

	if (!mapFunc->IsCallable())
		throw CRuntimeError(std::format("array.find expected \"callable\", but got \"{}\"", mapFunc->TypeAsString()));

	auto& vars = _this->GetShared()->GetVariables();

	IValue* result{ nullptr };

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
	return FindInternal(__this, ctx, args, true);
}
DEFINE_METHOD(FindLast) {
	START_METHOD(__this);
	return FindInternal(__this, ctx, args, false);
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
static inline IValue* FindIndexInternal(CArrayValue* _this, CRuntimeContext* const ctx, const IValues& args, bool findFirst)
{
	assert(args.size() == 1);
	auto& mapFunc = args.front();

	if (!mapFunc->IsCallable())
		throw CRuntimeError(std::format("array.findindex expected \"callable\", but got \"{}\"", mapFunc->TypeAsString()));

	auto& vars = _this->GetShared()->GetVariables();

	IValue* result{ nullptr };

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
	return FindIndexInternal(__this, ctx, args, true);
}
DEFINE_METHOD(FindLastIndex) {
	START_METHOD(__this);
	return FindIndexInternal(__this, ctx, args, false);
}

DEFINE_METHOD(Filter) 
{
	assert(args.size() == 1);
	auto& mapFunc = args.front();

	if (!mapFunc->IsCallable())
		throw CRuntimeError(std::format("array.filter expected \"callable\", but got \"{}\"", mapFunc->TypeAsString()));

	START_METHOD(__this);
	auto& vars = __this->GetShared()->GetVariables();

	IValues results;
	IValues call_args(1);
	IValue* exception{ nullptr };

	//result array
	for (const auto & var : vars) {
		call_args[0] = var->GetValue()->Copy();

		IValue* thisIteration = mapFunc->Call(ctx, call_args);

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

		for (auto& r : results) {
			if (r != exception)
				r->Release();
		}

		return exception;
	}

	return CArrayValue::Construct(std::move(results));
}

DEFINE_METHOD(Contains) 
{
	assert(args.size() == 1);
	auto& searchElement = args.front();

	START_METHOD(__this);
	auto& vars = __this->GetShared()->GetVariables();

	IValue* result{ nullptr };

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
	auto& delimiterValue = args.front();
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
	assert(args.size() == 1);
	auto& mapFunc = args.front();

	if (!mapFunc->IsCallable())
		throw CRuntimeError(std::format("array.all expected \"callable\", but got \"{}\"", mapFunc->TypeAsString()));

	START_METHOD(__this);
	auto& vars = __this->GetShared()->GetVariables();

	IValues call_args(1);
	IValue* exception{ nullptr };

	bool all = true;

	//result array
	for (const auto& var : vars) {
		call_args[0] = var->GetValue()->Copy();

		IValue* thisIteration = mapFunc->Call(ctx, call_args);

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
	assert(args.size() == 1);
	auto& mapFunc = args.front();

	if (!mapFunc->IsCallable())
		throw CRuntimeError(std::format("array.all expected \"callable\", but got \"{}\"", mapFunc->TypeAsString()));

	START_METHOD(__this);
	auto& vars = __this->GetShared()->GetVariables();

	IValues call_args(1);
	IValue* exception{ nullptr };

	bool any = false;

	//result array
	for (const auto& var : vars) {
		call_args[0] = var->GetValue()->Copy();

		IValue* thisIteration = mapFunc->Call(ctx, call_args);

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

DEFINE_METHOD(Slice) {

	START_METHOD(__this);
	auto& vars = __this->GetShared()->GetVariables();

	auto& a = args[0];
	auto& b = args[1];

	const auto CheckSanity = [](const IValue* v) {
		if (!v->IsIntegral())
			throw CRuntimeError(std::format("array.slice expected an integral value, but got \"{}\"", v->TypeAsString()));
		};

	CheckSanity(a);
	CheckSanity(b);

	auto start = a->ToInt();
	auto end = b->ToInt();

	auto len = vars.size();

	if (start >= end)
		throw CRuntimeError("array.slice expected start < end");

	end -= 1;

	const auto CheckRange = [&len](const auto value) {
		if (value < 0 || value >= static_cast<std::int64_t>(len))
			throw CRuntimeError("array.slice index out of range");
	};

	CheckRange(start);
	CheckRange(end);


	IValues valuesAsCopy;

	end += 1;
	for (auto i : std::views::iota(start, end))
		valuesAsCopy.push_back(vars[static_cast<std::size_t>(i)]->GetValue()->Copy()); //THE CAST IS SAFE!!!

	return CArrayValue::Construct(std::move(valuesAsCopy));
}

[[nodiscard]] bool doSort(CRuntimeContext* const ctx, IValue*& left, IValue*& right, IValue* const callback)
{

	IValues args = { left->Copy(), right->Copy() };
	auto returnValue = callback->Call(ctx, args);  // Call callback on swap

	if (CProgramRuntime::ExceptionThrown()) {
		throw CRuntimeError(std::format("array.sort callback must not throw"));
	}

	if (!returnValue->IsBooleanConvertible())
		throw CRuntimeError(std::format("array.sort expected a boolean return value", returnValue->TypeAsString()));

	const auto rtVal = returnValue->ToBoolean();
	returnValue->Release(); // nothing meaningful, release it

	return rtVal;
}
struct SortContext
{
	NONCOPYABLE(SortContext);
	SortContext(CRuntimeContext* const c, IValue* v) : ctx(c), callback(v){}

	CRuntimeContext* const ctx;
	std::size_t failedIterations{};
	IValue* const callback;
};

[[nodiscard]] auto Partition(SortContext& ctx, IValues& arr, std::size_t low, std::size_t high)
{
	auto pivot = arr[low];
	std::size_t left = low + 1;
	std::size_t right = high;

	//std::size_t prev_left = left;
	//std::size_t prev_right = right;

	while (true) {

		while (left <= high && (arr[left] == pivot || doSort(ctx.ctx, arr[left], pivot, ctx.callback))) left++;
		while (right > low && (arr[right] == pivot || doSort(ctx.ctx, pivot, arr[right], ctx.callback))) right--;

		if (left >= right) 
			break;

		std::swap(arr[left], arr[right]);

		//if (left == prev_left && right == prev_right) {
		//	throw CRuntimeError("array.sort wasn't making any progress due to a repeating condition.. probably an internal bug");
		//}

		//prev_left = left;
		//prev_right = right;
	}

	std::swap(arr[low], arr[right]);
	return right;
}



[[nodiscard]] void QuickSort(SortContext& ctx, IValues& values, std::size_t low, std::size_t high)
{

	if (low < high) {
		auto pi = Partition(ctx, values, low, high);
		if (pi > low) 
			QuickSort(ctx, values, low, pi - 1);
		if (pi + 1 < high) 
			QuickSort(ctx, values, pi + 1, high);
	}
}

DEFINE_METHOD(Sort)
{
	assert(args.size() == 1);
	auto& mapFunc = args.front();

	if (!mapFunc->IsCallable())
		throw CRuntimeError(std::format("array.sort expected \"callable\", but got \"{}\"", mapFunc->TypeAsString()));

	START_METHOD(__this);
	auto& vars = __this->GetShared()->GetVariables();

	IValues valuesAsCopy(vars.size());

	for (auto i = size_t(0); auto & var : vars)
		valuesAsCopy[i++] = var->GetValue()->Copy();

	SortContext context(ctx, mapFunc);
	QuickSort(context, valuesAsCopy, 0, valuesAsCopy.size() - 1);

	return CArrayValue::Construct(std::move(valuesAsCopy));
}

DEFINE_METHOD(Resize)
{
	assert(args.size() == 1);
	auto& value = args.front();

	if (!value->IsIntegral())
		throw CRuntimeError(std::format("array.resize expected \"integer\", but got \"{}\"", value->TypeAsString()));


	START_METHOD(__this);
	auto& vars = __this->GetShared()->GetVariables();

	auto intVal = value->ToInt();

	if(intVal < 0)
		throw CRuntimeError(std::format("array.resize out of range < 0 ({})", value->TypeAsString()));

	const auto uintval = static_cast<std::size_t>(intVal);
	const auto oldSize = vars.size();
	if (uintval > oldSize) {
		//allocate new 
		vars.reserve(uintval);

		const auto delta = uintval - oldSize;

		for ([[maybe_unused]] auto i : std::views::iota(0u, delta)) {
			auto v = CProgramRuntime::AcquireNewVariable();
			v->SetValue(CProgramRuntime::AcquireNewValue<IValue>());
			vars.push_back(v);
		}
	} else if (uintval < oldSize) {
		//shrink me :3

		for (const auto i : std::views::iota(uintval, oldSize)) {
			vars[i]->Release();
		}

		vars.resize(uintval);
	}

	return __this->Copy();
}

DEFINE_METHOD(Fill)
{
	assert(args.size() == 1);
	auto& value = args.front();

	START_METHOD(__this);
	auto& vars = __this->GetShared()->GetVariables();

	for (auto& v : vars) {
		v->SetValue(value->Copy());
	}

	return __this->Copy();
}
