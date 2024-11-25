#include "operand.hpp"
#include "operand_immediate.hpp"

#include "linter/token.hpp"
#include "linter/expressions/ast.hpp"

#include <charconv>
#include <cassert>

std::unique_ptr<IOperand> CLinterOperand::ParseImmediate(){
	auto& oldIter = m_iterPos;
	auto&& v = std::make_unique<CImmediateOperand>(*m_iterPos);
	v->m_oCodePosition = (*oldIter)->m_oSourcePosition;
	std::advance(m_iterPos, 1);
	return v;
}

UniqueAST CImmediateOperand::ToAST(){
	return std::make_unique<ConstantASTNode>(m_oCodePosition, ToData(), GetImmediateType());
}

#pragma pack(push)
#pragma warning(disable : 4061)
#pragma warning(disable : 4062)
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
		return sizeof(std::int64_t);
	case t_double:
		return sizeof(double);
	case t_string:
		return m_pToken->Source().size();
	case t_callable:
		return sizeof(void*);
	default:
		assert(false);
		return 0u;
	}
}

std::string CImmediateOperand::ToData() const noexcept
{
	std::string result;
	auto& string = m_pToken->Source();

	switch (GetImmediateType()) {
	case t_undefined:
		return "";
	case t_boolean:
		return std::string(1, m_pToken->Type() == TokenType::tt_true ? '\x01' : '\x00');
	case t_int:
		result = std::string(sizeof(std::int64_t), 0);
		std::from_chars(string.c_str(), string.c_str() + string.size(), reinterpret_cast<std::int64_t&>(*result.data()));
		break;
	case t_double:
		result = std::string(sizeof(double), 0);
		std::from_chars(string.c_str(), string.c_str() + string.size(), reinterpret_cast<double&>(*result.data()));
		break;
	case t_string:
		return string;
	}

	return result;
}

#pragma pack(pop)