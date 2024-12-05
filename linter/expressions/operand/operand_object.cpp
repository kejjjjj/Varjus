#include "linter/context.hpp"
#include "linter/error.hpp"
#include "linter/expressions/ast.hpp"
#include "linter/expressions/expression.hpp"
#include "linter/functions/stack.hpp"
#include "linter/token.hpp"

#include "operand.hpp"
#include "operand_object.hpp"

#include <cassert>

CKeyValue::CKeyValue(std::size_t k, UniqueAST&& ast)
	: m_uKey(k), m_pValue(std::move(ast)) {
}

std::unique_ptr<CKeyValue> CLinterOperand::ParseKeyValue(std::optional<PairMatcher> eoe)
{
	if (!IsEndOfBuffer() && EndOfExpression(eoe, m_iterPos)) {
		return nullptr;
	}

	if (IsEndOfBuffer() || (*m_iterPos)->Type() != tt_name) {
		CLinterErrors::PushError("expected an identifier", GetIteratorSafe()->m_oSourcePosition);
		return nullptr;
	}
	auto& identifier = (*m_iterPos)->Source();

	std::advance(m_iterPos, 1); // skip identifier

	if (IsEndOfBuffer() || !(*m_iterPos)->IsOperator(p_colon)) {
		CLinterErrors::PushError("expected \":\"", GetIteratorSafe()->m_oSourcePosition);
		return nullptr;
	}

	std::advance(m_iterPos, 1); // skip :

	CLinterExpression expr(m_iterPos, m_iterEnd, m_pScope, m_pOwner);
	if (!expr.ParseInternal(eoe, nullptr, evaluate_singular)) //only parse this key
		return nullptr;


	if (!IsEndOfBuffer() && (*m_iterPos)->IsOperator(p_comma)) {
		std::advance(m_iterPos, 1); //skip the comma
	}

	return std::make_unique<CKeyValue>(
		m_pOwner->GetContext()->m_oAllMembers[identifier],
		expr.ToMergedAST()
	);

}
std::unique_ptr<IOperand> CLinterOperand::ParseObject()
{
	std::advance(m_iterPos, 1); // skip {

	auto& oldIter = m_iterPos;

	if (IsEndOfBuffer() || (*m_iterPos)->IsOperator(p_curlybracket_close)) {
		std::advance(m_iterPos, 1); // skip }
		return std::make_unique<CObjectOperand>();
	}

	auto object = std::make_unique<CObjectOperand>();

	while (auto v = ParseKeyValue(PairMatcher(p_curlybracket_open)))
		object->m_oAttributes.emplace_back(std::make_pair(v->m_uKey, std::move(v->m_pValue)));

	assert(!IsEndOfBuffer() && (*m_iterPos)->IsOperator(p_curlybracket_close));
	std::advance(m_iterPos, 1); //skip }

	object->m_oCodePosition = (*oldIter)->m_oSourcePosition;
	return object;

}

UniqueAST CObjectOperand::ToAST() {
	return std::make_unique<ObjectASTNode>(m_oCodePosition, std::move(m_oAttributes));
}

CObjectOperand::CObjectOperand(VectorOf<KeyValue<std::size_t, UniqueAST>>&& ptr) : m_oAttributes(std::move(ptr)) {}
CObjectOperand::~CObjectOperand() = default;