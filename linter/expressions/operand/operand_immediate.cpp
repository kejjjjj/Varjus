#include "operand.hpp"
#include "operand_immediate.hpp"

#include "linter/token.hpp"
#include "linter/expressions/ast.hpp"

#include <charconv>
#include <cassert>

using namespace Varjus;

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

#pragma pack(push)
VARJUS_WARNING_DISABLE(4061)
VARJUS_WARNING_DISABLE(4062)
Varjus::EValueType CImmediateOperand::GetImmediateType() const noexcept
{
	switch (m_pToken->Type()) {
	case TokenType::tt_undefined:
		return Varjus::EValueType::t_undefined;
	case TokenType::tt_false:
	case TokenType::tt_true:
		return Varjus::EValueType::t_boolean;
	case TokenType::tt_int:
		return Varjus::EValueType::t_int;
	case TokenType::tt_uint:
		return Varjus::EValueType::t_uint;
	case TokenType::tt_double:
		return Varjus::EValueType::t_double;
	case TokenType::tt_string:
		return Varjus::EValueType::t_string;
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
#include "fs/fs_io.hpp"
#endif

VarjusString CImmediateOperand::ToData() const noexcept
{
	if (GetImmediateType() == t_string)
		return m_pToken->Source();


	VarjusString result;

#ifdef UNICODE
	std::string string = LocaleConverter::u16string_to_ansi(m_pToken->Source());
#else
	VarjusString string = m_pToken->Source();
#endif
	VarjusDouble dvalue{};


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
		dvalue = std::strtod(string.c_str(), nullptr);
		std::memcpy(result.data(), &dvalue, sizeof(VarjusDouble));
		break;
	case t_string:
	default:
		assert(false);
	}
	return result;

}

#pragma pack(pop)