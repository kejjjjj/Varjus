#include "operand.hpp"
#include "operand_immediate.hpp"

#include "linter/token.hpp"
#include "linter/expressions/ast.hpp"

#ifdef OPTIMIZATIONS
#include "linter/optimizations/optimizations.hpp"
#endif

#include <charconv>
#include <cassert>

std::unique_ptr<IOperand> CLinterOperand::ParseImmediate(){
	auto& oldIter = m_iterPos;
	auto&& v = std::make_unique<CImmediateOperand>(*m_iterPos);
	v->m_oCodePosition = (*oldIter)->m_oSourcePosition;
	std::advance(m_iterPos, 1);
	return v;
}

ASTNode CImmediateOperand::ToAST(){
	return std::make_shared<ConstantASTNode>(m_oCodePosition, ToData(), GetImmediateType());
}

#ifdef OPTIMIZATIONS

IConstEvalValue* ConstantASTNode::GetConstEval([[maybe_unused]]CMemory* const owner) noexcept
{
	switch (m_eDataType) {
	case t_undefined:
		return COptimizationValues::AcquireNewValue<IConstEvalValue>();
	case t_int:
		return COptimizationValues::AcquireNewValue<CConstEvalIntValue>(*reinterpret_cast<VarjusInt*>((char*)m_pConstant.data()));
	case t_boolean:
		return COptimizationValues::AcquireNewValue<CConstEvalBooleanValue>(m_pConstant[0] == '\x01');
	case t_double:
		return COptimizationValues::AcquireNewValue<CConstEvalDoubleValue>(*reinterpret_cast<VarjusDouble*>((char*)m_pConstant.data()));
	case t_string:
		return COptimizationValues::AcquireNewValue<CConstEvalStringValue>(m_pConstant);
	case t_callable:
	case t_array:
	case t_object:
	default:
		assert(false);
		return nullptr;
	}
}

#endif

#pragma pack(push)
WARNING_DISABLE(4061)
WARNING_DISABLE(4062)
EValueType CImmediateOperand::GetImmediateType() const noexcept
{
	switch (m_pToken->Type()) {
	case TokenType::tt_undefined:
		return EValueType::t_undefined;
	case TokenType::tt_false:
	case TokenType::tt_true:
		return EValueType::t_boolean;
	case TokenType::tt_int:
		return EValueType::t_int;
	case TokenType::tt_uint:
		return EValueType::t_uint;
	case TokenType::tt_double:
		return EValueType::t_double;
	case TokenType::tt_string:
		return EValueType::t_string;
	default:
		assert(false);
		return t_undefined;
	}
}
std::size_t CImmediateOperand::GetImmediateSize() const noexcept
{
	auto type = GetImmediateType();

	switch (type) {
	case t_undefined:
		return std::size_t(0);
	case t_boolean:
		return sizeof(bool);
	case t_int:
		return sizeof(VarjusInt);
	case t_uint:
		return sizeof(VarjusUInt);
	case t_double:
		return sizeof(VarjusDouble);
	case t_string:
		return m_pToken->Source().size();
	case t_callable:
		return sizeof(void*);
	default:
		assert(false);
		return 0u;
	}
}

#ifdef UNICODE
#include <locale>
inline std::string ToNarrow(const std::wstring& wide_str) {
	std::locale loc;

	const auto str = wide_str.data();

	auto len = wide_str.length();

	std::string narrow_str;
	narrow_str.resize(len);

	std::use_facet<std::ctype<wchar_t>>(loc).narrow(str, str + len, '?', &narrow_str[0]);

	return narrow_str;
}

inline std::wstring ToWide(const std::string& string) {

	std::locale loc;

	const auto str = string.data();

	auto len = string.length();

	std::wstring wide_str;
	wide_str.resize(len);

	std::use_facet<std::ctype<wchar_t>>(loc).widen(str, str + len, wide_str.data());

	return wide_str;
}

#endif

VarjusString CImmediateOperand::ToData() const noexcept
{
	if (GetImmediateType() == t_string)
		return m_pToken->Source();


	VarjusString result;

#ifdef UNICODE
	std::string string = ToNarrow(m_pToken->Source());
#else
	VarjusString string = m_pToken->Source();
#endif


	switch (GetImmediateType()) {
	case t_undefined:
		return VSL("");
	case t_boolean:
		return VarjusString(1, m_pToken->Type() == TokenType::tt_true ? VarjusChar('\x01') : VarjusChar('\x00'));
	case t_int:
		result = VarjusString(sizeof(VarjusInt), 0);
		std::from_chars(string.c_str(), string.c_str() + string.size(), reinterpret_cast<VarjusInt&>(*result.data()));
		break;
	case t_uint:
		result = VarjusString(sizeof(VarjusUInt), 0);
		std::from_chars(string.c_str(), string.c_str() + string.size(), reinterpret_cast<VarjusUInt&>(*result.data()));
		break;
	case t_double:
		result = VarjusString(sizeof(VarjusDouble), 0);
		std::from_chars(string.c_str(), string.c_str() + string.size(), reinterpret_cast<VarjusDouble&>(*result.data()));
		break;
	case t_string:
		break;
	default:
		assert(false);
	}

	return result;

}

#pragma pack(pop)