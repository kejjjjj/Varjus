#include "varjus_api/types/types.hpp"
#include "varjus_api/types/operators/default_operators.hpp"

#include "varjus_api/internal/runtime.hpp"
#include "varjus_api/internal/variables.hpp"
#include "runtime/exceptions/exception.hpp"
#include "varjus_api/internal/structure.hpp"

#include <algorithm>
#include <ranges>
#include <stack>
#include <numeric>

[[nodiscard]] CArrayValue* GetThisArray(IValue* _this) {
	return _this->ToArray();
}

FORWARD_DECLARE_METHOD(Push);
FORWARD_DECLARE_METHOD(PushFront);
FORWARD_DECLARE_METHOD(Pop);
FORWARD_DECLARE_METHOD(PopFront);
FORWARD_DECLARE_METHOD(Transform);
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
FORWARD_DECLARE_METHOD(ForEach);
FORWARD_DECLARE_METHOD(Accumulate);


std::unique_ptr<BuiltInMethod_t> CArrayValue::ConstructMethods(CProgramInformation* const info)
{
	auto m_oMethods = std::make_unique<BuiltInMethod_t>(info);

	m_oMethods->AddMethod(VSL("push"),            Push,          1u);
	m_oMethods->AddMethod(VSL("push_front"),      PushFront,     1u);
	m_oMethods->AddMethod(VSL("pop"),             Pop,           0u);
	m_oMethods->AddMethod(VSL("pop_front"),       PopFront,      0u);
	m_oMethods->AddMethod(VSL("transform"),       Transform,     1u);
	m_oMethods->AddMethod(VSL("find"),            Find,          1u);
	m_oMethods->AddMethod(VSL("find_last"),       FindLast,      1u);
	m_oMethods->AddMethod(VSL("find_index"),      FindIndex,     1u);
	m_oMethods->AddMethod(VSL("find_last_index"), FindLastIndex, 1u);
	m_oMethods->AddMethod(VSL("filter"),          Filter,        1u);
	m_oMethods->AddMethod(VSL("contains"),        Contains,      1u);
	m_oMethods->AddMethod(VSL("reversed"),        Reversed,      0u);
	m_oMethods->AddMethod(VSL("join"),            Join,          1u);
	m_oMethods->AddMethod(VSL("all"),             All,           1u);
	m_oMethods->AddMethod(VSL("any"),             Any,           1u);
	m_oMethods->AddMethod(VSL("slice"),	          Slice,         2u);
	m_oMethods->AddMethod(VSL("sort"),            Sort,          1u);
	m_oMethods->AddMethod(VSL("resize"),          Resize,        1u);
	m_oMethods->AddMethod(VSL("fill"),            Fill,          1u);
	m_oMethods->AddMethod(VSL("for_each"),        ForEach,       1u);
	m_oMethods->AddMethod(VSL("accumulate"),      Accumulate,    2u);

	return m_oMethods;
}

FORWARD_DECLARE_PROPERTY(ArrayLength);

std::unique_ptr<BuiltInProperty_t> CArrayValue::ConstructProperties(CProgramInformation* const info)
{
	auto m_oProperties = std::make_unique<BuiltInProperty_t>(info);
	m_oProperties->AddProperty(VSL("length"), ArrayLength);

	return m_oProperties;
}


DEFINE_PROPERTY(ArrayLength) {
	auto __this = GetThisArray(_this);
	return CUIntValue::Construct(ctx->m_pRuntime, static_cast<VarjusUInt>(__this->Internal()->Length()));
}

DEFINE_METHOD(Push, args)
{
	auto __this = GetThisArray(_this);
	auto& vars = __this->GetVariables();
	auto& newVar = vars.emplace_back(CVariable::Construct(ctx->m_pRuntime, args.front()->Copy()));
	return newVar->GetValue()->Copy();
}
DEFINE_METHOD(PushFront, args)
{
	auto __this = GetThisArray(_this);
	auto& vars = __this->GetVariables();

	auto it = vars.insert(vars.begin(), CVariable::Construct(ctx->m_pRuntime, args.front()->Copy()));
	return (*it)->GetValue()->Copy();
}

DEFINE_METHOD(Pop, args)
{

	auto __this = GetThisArray(_this);
	auto& vars = __this->GetVariables();

	if (vars.empty())
		throw CRuntimeError(ctx->m_pRuntime, VSL("attempted to pop an empty array"));

	auto& back = vars.back();

	auto copy = back->GetValue()->Copy();
	back->Release();
	vars.pop_back();

	return copy;
}
DEFINE_METHOD(PopFront, args)
{

	auto __this = GetThisArray(_this);
	auto& vars = __this->GetVariables();

	if (vars.empty())
		throw CRuntimeError(ctx->m_pRuntime, VSL("attempted to pop_front an empty array"));

	auto& front = vars.front();

	auto copy = front->GetValue()->Copy();
	front->Release();
	vars.erase(vars.begin());

	return copy;
}

DEFINE_METHOD(Transform, args)
{
	assert(args.size() == 1);
	auto& mapFunc = args.front();

	if (!mapFunc->IsCallable())
		throw CRuntimeError(ctx->m_pRuntime, fmt::format(VSL("array.transform expected \"callable\", but got \"{}\""), mapFunc->TypeAsString()));

	auto __this = GetThisArray(_this);
	auto& vars = __this->GetVariables();

	IValues results(vars.size());
	IValues call_args(1);

	//result array
	for (auto i = std::size_t(0); const auto& var : vars) {
		call_args[0] = var->GetValue()->Copy();
		results[i++] = mapFunc->Call(ctx, call_args);

		//because of course someone will throw an exception :x
		if (ctx->m_pRuntime->ExceptionThrown()) {
			results.resize(i - 1);
			break;
		}

	}

	if (ctx->m_pRuntime->ExceptionThrown()) {

		for (auto& r : results) {
			if(r != ctx->m_pRuntime->GetExceptionValue())
				r->Release();
		}

		return ctx->m_pRuntime->GetExceptionValue();
	}

	
	return CArrayValue::Construct(ctx->m_pRuntime, std::move(results));
}
static inline IValue* FindTestValue(CRuntimeContext* const ctx, IValue* const mapFunc, CVariable* const var)
{
	IValues args(1);
	args[0] = var->GetValue()->Copy();

	IValue* thisIteration = mapFunc->Call(ctx, args);
	IValue* result{ nullptr };

	if (ctx->m_pRuntime->ExceptionThrown()) {
		return ctx->m_pRuntime->GetExceptionValue();
	}

	if (!thisIteration->IsBooleanConvertible())
		throw CRuntimeError(ctx->m_pRuntime, fmt::format(VSL("array.find expected a boolean return value"), mapFunc->TypeAsString()));

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
		throw CRuntimeError(ctx->m_pRuntime, fmt::format(VSL("array.find expected \"callable\", but got \"{}\""), mapFunc->TypeAsString()));

	auto& vars = _this->GetVariables();

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

	if (ctx->m_pRuntime->ExceptionThrown()) {
		return ctx->m_pRuntime->GetExceptionValue();
	}

	if (!result)
		return IValue::Construct(ctx->m_pRuntime); //didn't find, return undefined

	return result->Copy();
}

DEFINE_METHOD(Find, args) {
	auto __this = GetThisArray(_this);
	return FindInternal(__this, ctx, args, true);
}
DEFINE_METHOD(FindLast, args) {
	auto __this = GetThisArray(_this);
	return FindInternal(__this, ctx, args, false);
}

static inline IValue* FindTestValueIndex(CRuntimeContext* const ctx, IValue* const mapFunc, CVariable* const var, std::size_t i)
{
	IValues args(1);
	args[0] = var->GetValue()->Copy();

	IValue* thisIteration = mapFunc->Call(ctx, args);
	IValue* result{ nullptr };

	if (ctx->m_pRuntime->ExceptionThrown()) {
		return ctx->m_pRuntime->GetExceptionValue();
	}

	if (!thisIteration->IsBooleanConvertible())
		throw CRuntimeError(ctx->m_pRuntime, fmt::format(VSL("array.find expected a boolean return value"), mapFunc->TypeAsString()));

	if (thisIteration->ToBoolean()) {
		result = CIntValue::Construct(ctx->m_pRuntime, static_cast<VarjusInt>(i));
	}

	thisIteration->Release(); // nothing meaningful, release it
	return result;
}
static inline IValue* FindIndexInternal(CArrayValue* _this, CRuntimeContext* const ctx, const IValues& args, bool findFirst)
{
	assert(args.size() == 1);
	auto& mapFunc = args.front();

	if (!mapFunc->IsCallable())
		throw CRuntimeError(ctx->m_pRuntime, fmt::format(VSL("array.findindex expected \"callable\", but got \"{}\""), mapFunc->TypeAsString()));

	auto& vars = _this->GetVariables();

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

	if (ctx->m_pRuntime->ExceptionThrown()) {
		return ctx->m_pRuntime->GetExceptionValue();
	}

	if (!result)
		return CIntValue::Construct(ctx->m_pRuntime, -1); //didn't find, return -1

	return result;
}
DEFINE_METHOD(FindIndex, args) {
	auto __this = GetThisArray(_this);
	return FindIndexInternal(__this, ctx, args, true);
}
DEFINE_METHOD(FindLastIndex, args) {
	auto __this = GetThisArray(_this);
	return FindIndexInternal(__this, ctx, args, false);
}

DEFINE_METHOD(Filter, args)
{
	assert(args.size() == 1);
	auto& mapFunc = args.front();

	if (!mapFunc->IsCallable())
		throw CRuntimeError(ctx->m_pRuntime, fmt::format(VSL("array.filter expected \"callable\", but got \"{}\""), mapFunc->TypeAsString()));

	auto __this = GetThisArray(_this);
	auto& vars = __this->GetVariables();

	IValues results;
	IValues call_args(1);

	//result array
	for (const auto & var : vars) {
		call_args[0] = var->GetValue()->Copy();

		IValue* thisIteration = mapFunc->Call(ctx, call_args);

		if (ctx->m_pRuntime->ExceptionThrown()) {
			break;
		}

		if (!thisIteration->IsBooleanConvertible())
			throw CRuntimeError(ctx->m_pRuntime, fmt::format(VSL("array.filter expected a boolean return value"), mapFunc->TypeAsString()));

		if (thisIteration->ToBoolean())
			results.push_back(var->GetValue()->Copy());

		thisIteration->Release(); // nothing meaningful, release it

		if (ctx->m_pRuntime->ExceptionThrown())
			break;
	}

	if (ctx->m_pRuntime->ExceptionThrown()) {

		for (auto& r : results) {
			if (r != ctx->m_pRuntime->GetExceptionValue())
				r->Release();
		}

		return ctx->m_pRuntime->GetExceptionValue();
	}

	return CArrayValue::Construct(ctx->m_pRuntime, std::move(results));
}

DEFINE_METHOD(Contains, args)
{
	assert(args.size() == 1);
	auto& searchElement = args.front();

	auto __this = GetThisArray(_this);
	auto& vars = __this->GetVariables();

	IValue* result{ nullptr };

	for (const auto& var : vars) {
		IValue* thisIteration = OP_STRICT_EQUALITY(ctx->m_pRuntime, var->GetValue(), searchElement);

		if (thisIteration->ToBoolean()) {
			result = thisIteration;
			break;
		}

		thisIteration->Release(); // nothing meaningful, release it
	}

	if (!result)
		return CBooleanValue::Construct(ctx->m_pRuntime, false);

	return result;
}
DEFINE_METHOD(Reversed, args) {
	
	auto __this = GetThisArray(_this);
	auto& vars = __this->GetVariables();
	IValues valuesAsCopy(vars.size());

	for (auto i = size_t(0); auto& var : vars)
		valuesAsCopy[i++] = var->GetValue()->Copy();
	
	std::reverse(valuesAsCopy.begin(), valuesAsCopy.end());

	return CArrayValue::Construct(ctx->m_pRuntime, std::move(valuesAsCopy));
}


VarjusString JoinStrings(const VectorOf<VarjusString>& strings, const VarjusString& delimiter) {
	VarjusString result;
	for (auto i = std::size_t(0); i < strings.size(); ++i) {
		result += strings[i];
		if (i != strings.size() - std::size_t(1)) {
			result += delimiter;
		}
	}
	return result;
}

DEFINE_METHOD(Join, args)
{
	auto& delimiterValue = args.front();
	if (delimiterValue->Type() != t_string)
		throw CRuntimeError(ctx->m_pRuntime, fmt::format(VSL("array.join expected a string parameter, but got \"{}\""), delimiterValue->TypeAsString()));

	VectorOf<VarjusString> stringValues;
	auto __this = GetThisArray(_this);
	auto& vars = __this->GetVariables();

	for (auto& var : vars) {
		auto& value = var->GetValue();
		if (value->Type() != t_string)
			throw CRuntimeError(ctx->m_pRuntime, fmt::format(VSL("array.join called on an array that contains \"{}\" instead of \"string\""), 
				delimiterValue->TypeAsString()));

		stringValues.push_back(value->ToString());
	}

	return CStringValue::Construct(ctx->m_pRuntime, JoinStrings(stringValues, delimiterValue->ToString()));
}

DEFINE_METHOD(All, args)
{
	assert(args.size() == 1);
	auto& mapFunc = args.front();

	if (!mapFunc->IsCallable())
		throw CRuntimeError(ctx->m_pRuntime, fmt::format(VSL("array.all expected \"callable\", but got \"{}\""), mapFunc->TypeAsString()));

	auto __this = GetThisArray(_this);
	auto& vars = __this->GetVariables();

	IValues call_args(1);

	bool all = true;

	//result array
	for (const auto& var : vars) {
		call_args[0] = var->GetValue()->Copy();

		IValue* thisIteration = mapFunc->Call(ctx, call_args);

		if (ctx->m_pRuntime->ExceptionThrown()) {
			break;
		}

		if (!thisIteration->IsBooleanConvertible())
			throw CRuntimeError(ctx->m_pRuntime, fmt::format(VSL("array.filter expected a boolean return value"), mapFunc->TypeAsString()));

		if (!thisIteration->ToBoolean())
			all = false;

		thisIteration->Release(); // nothing meaningful, release it

		if (!all || ctx->m_pRuntime->ExceptionThrown())
			break;
	}

	if (ctx->m_pRuntime->ExceptionThrown()) {
		return ctx->m_pRuntime->GetExceptionValue();
	}

	return CBooleanValue::Construct(ctx->m_pRuntime, all);
}
DEFINE_METHOD(Any, args)
{
	assert(args.size() == 1);
	auto& mapFunc = args.front();

	if (!mapFunc->IsCallable())
		throw CRuntimeError(ctx->m_pRuntime, fmt::format(VSL("array.all expected \"callable\", but got \"{}\""), mapFunc->TypeAsString()));

	auto __this = GetThisArray(_this);
	auto& vars = __this->GetVariables();

	IValues call_args(1);

	bool any = false;

	//result array
	for (const auto& var : vars) {
		call_args[0] = var->GetValue()->Copy();

		IValue* thisIteration = mapFunc->Call(ctx, call_args);

		if (ctx->m_pRuntime->ExceptionThrown()) {
			break;
		}

		if (!thisIteration->IsBooleanConvertible())
			throw CRuntimeError(ctx->m_pRuntime, fmt::format(VSL("array.filter expected a boolean return value"), mapFunc->TypeAsString()));

		if (thisIteration->ToBoolean())
			any = true;

		thisIteration->Release(); // nothing meaningful, release it

		if (any || ctx->m_pRuntime->ExceptionThrown())
			break;
	}

	if (ctx->m_pRuntime->ExceptionThrown()) {
		return ctx->m_pRuntime->GetExceptionValue();
	}

	return CBooleanValue::Construct(ctx->m_pRuntime, any);
}

DEFINE_METHOD(Slice, args) {

	auto __this = GetThisArray(_this);
	auto& vars = __this->GetVariables();

	auto& a = args[0];
	auto& b = args[1];

	const auto CheckSanity = [&ctx](const IValue* v) {
		if (!v->IsIntegral())
			throw CRuntimeError(ctx->m_pRuntime, fmt::format(VSL("array.slice expected an integral value, but got \"{}\""), v->TypeAsString()));
		};

	CheckSanity(a);
	CheckSanity(b);

	auto start = a->ToInt();
	auto end = b->ToInt();

	auto len = vars.size();

	if (start >= end)
		throw CRuntimeError(ctx->m_pRuntime, VSL("array.slice expected start < end"));

	end -= 1;

	const auto CheckRange = [&len, &ctx](const auto value) {
		if (value < 0 || value >= static_cast<VarjusInt>(len))
			throw CRuntimeError(ctx->m_pRuntime, VSL("array.slice index out of range"));
	};

	CheckRange(start);
	CheckRange(end);


	IValues valuesAsCopy;

	end += 1;
	for (auto i : std::views::iota(start, end))
		valuesAsCopy.push_back(vars[static_cast<std::size_t>(i)]->GetValue()->Copy()); //THE CAST IS SAFE!!!

	return CArrayValue::Construct(ctx->m_pRuntime, std::move(valuesAsCopy));
}
[[nodiscard]] bool doSort(CRuntimeContext* const ctx, IValue*& left, IValue*& right, IValue* const callback);

void IterativeQuickSort(CRuntimeContext* const ctx, IValues& vars, IValue* const callback)
{
	if (vars.empty())
		return;

	std::stack<std::pair<VarjusInt, VarjusInt>> stk;
	stk.push({ VarjusInt(0), static_cast<VarjusInt>(vars.size()) - VarjusInt(1) });

	while (!stk.empty()) {

		auto [low, high] = stk.top();
		stk.pop();

		if (low >= high) 
			continue;

		auto& pivot = vars[VarjusUInt(high)];
		auto i = low - VarjusInt(1);

		for (auto j = low; j < high; j++) {

			if (doSort(ctx, vars[VarjusUInt(j)], pivot, callback))
				std::swap(vars[VarjusUInt(++i)], vars[VarjusUInt(j)]);

			if (ctx->m_pRuntime->ExceptionThrown())
				return;

		}

		std::swap(vars[VarjusUInt(i + VarjusInt(1))], vars[VarjusUInt(high)]);
		auto pivotIndex = i + VarjusInt(1);

		stk.push({ low, pivotIndex - VarjusInt(1) });
		stk.push({ pivotIndex + VarjusInt(1), high });
	}
}

[[nodiscard]] bool doSort(CRuntimeContext* const ctx, IValue*& left, IValue*& right, IValue* const callback)
{

	IValues args = { left->Copy(), right->Copy() };
	auto returnValue = callback->Call(ctx, args);  // Call callback on swap

	if (ctx->m_pRuntime->ExceptionThrown()) {
		return false;
	}

	if (returnValue->Type() != t_boolean)
		throw CRuntimeError(ctx->m_pRuntime, fmt::format(VSL("array.sort expected a boolean return value"), returnValue->TypeAsString()));

	const auto rtVal = returnValue->ToBoolean();
	returnValue->Release(); // nothing meaningful, release it

	return rtVal;
}

DEFINE_METHOD(Sort, args)
{
	assert(args.size() == 1);
	auto& mapFunc = args.front();

	if (!mapFunc->IsCallable())
		throw CRuntimeError(ctx->m_pRuntime, fmt::format(VSL("array.sort expected \"callable\", but got \"{}\""), mapFunc->TypeAsString()));

	auto __this = GetThisArray(_this);
	auto& vars = __this->GetVariables();

	IValues valuesAsCopy(vars.size());

	for (auto i = size_t(0); auto & var : vars)
		valuesAsCopy[i++] = var->GetValue();

	IterativeQuickSort(ctx, valuesAsCopy, mapFunc);

	if (ctx->m_pRuntime->ExceptionThrown())
		return ctx->m_pRuntime->GetExceptionValue();

	for (auto i = std::size_t(0); auto & v : vars)
		std::swap(v->GetValue(), valuesAsCopy[i++]);

	return __this->Copy();
}

DEFINE_METHOD(Resize, args)
{
	assert(args.size() == 1);
	auto& value = args.front();

	if (!value->IsIntegral())
		throw CRuntimeError(ctx->m_pRuntime, fmt::format(VSL("array.resize expected \"integer\", but got \"{}\""), value->TypeAsString()));


	auto __this = GetThisArray(_this);
	auto& vars = __this->GetVariables();

	auto intVal = value->ToInt();

	if(intVal < 0)
		throw CRuntimeError(ctx->m_pRuntime, fmt::format(VSL("array.resize out of range < 0 ({})"), value->TypeAsString()));

	const auto uintval = static_cast<std::size_t>(intVal);
	const auto oldSize = vars.size();
	if (uintval > oldSize) {
		//allocate new 
		vars.reserve(uintval);

		const auto delta = uintval - oldSize;

		for ([[maybe_unused]] auto i : std::views::iota(0u, delta)) {
			vars.push_back(CVariable::Construct(ctx->m_pRuntime, IValue::Construct(ctx->m_pRuntime)));
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

DEFINE_METHOD(Fill, args)
{
	assert(args.size() == 1);
	auto& value = args.front();

	auto __this = GetThisArray(_this);
	auto& vars = __this->GetVariables();

	for (auto& v : vars) {
		v->SetValue(value->Copy());
	}

	return __this->Copy();
}
DEFINE_METHOD(ForEach, args)
{
	assert(args.size() == 1);
	auto& mapFunc = args.front();

	if (!mapFunc->IsCallable())
		throw CRuntimeError(ctx->m_pRuntime, fmt::format(VSL("array.for_each expected \"callable\", but got \"{}\""), mapFunc->TypeAsString()));

	auto __this = GetThisArray(_this);
	auto& vars = __this->GetVariables();

	IValues call_args(1);

	//result array
	for (const auto& var : vars) {
		call_args[0] = var->GetValue()->Copy();

		IValue* thisIteration = mapFunc->Call(ctx, call_args);

		if (ctx->m_pRuntime->ExceptionThrown()) {
			break;
		}

		thisIteration->Release(); // nothing meaningful, release it

		if (ctx->m_pRuntime->ExceptionThrown())
			break;
	}

	if (ctx->m_pRuntime->ExceptionThrown()) {
		return ctx->m_pRuntime->GetExceptionValue();
	}

	return IValue::Construct(ctx->m_pRuntime);
}

DEFINE_METHOD(Accumulate, args)
{
	auto& mapFunc = args.front();

	if (!mapFunc->IsCallable())
		throw CRuntimeError(ctx->m_pRuntime, fmt::format(VSL("array.accumulate 1. expected \"callable\", but got \"{}\""), mapFunc->TypeAsString()));


	auto __this = GetThisArray(_this);
	auto& vars = __this->GetVariables();

	if(vars.empty())
		return IValue::Construct(ctx->m_pRuntime);

	IValues call_args(2);
	IValue* accumulator = args[1]->Copy();

	//result array
	for (const auto& var : vars) {
		call_args[0] = accumulator;
		call_args[1] = var->GetValue()->Copy();

		IValue* thisIteration = mapFunc->Call(ctx, call_args);

		if (ctx->m_pRuntime->ExceptionThrown()) {
			break;
		}

		accumulator = thisIteration->Copy();
		thisIteration->Release();

	}


	if (ctx->m_pRuntime->ExceptionThrown()) {
		return ctx->m_pRuntime->GetExceptionValue();
	}

	return accumulator;
}