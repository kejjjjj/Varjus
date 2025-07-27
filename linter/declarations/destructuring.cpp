#include "destructuring.hpp"
#include "linter/functions/stack.hpp"
#include "linter/modules/module.hpp"
#include "linter/punctuation.hpp"
#include "linter/token.hpp"
#include "variable_declarations.hpp"
#include "linter/expressions/ast.hpp"

#include <cassert>

CDestructuringLinter::CDestructuringLinter(LinterIterator& pos, LinterIterator& end, 
	const WeakScope& scope, CMemory* const stack, bool isConst) :
	CLinterSingle(pos, end), m_pScope(scope), m_pOwner(stack), m_bConst(isConst) {
	assert(m_pOwner != nullptr);
}
CDestructuringLinter::~CDestructuringLinter() = default;


Varjus::Success CDestructuringLinter::Parse()
{
	const auto token = GetIteratorSafe();

	if (IsEndOfBuffer() || !IsDestructuringToken(token)) {
		m_pOwner->GetModule()->PushError(VSL("expected \"[\" or \"{\""), token->m_oSourcePosition);
		return Varjus::failure;
	}

	Punctuation p{};

	if (token->IsOperator(p_bracket_open)) {
		m_oParseData.m_oData = DestructuredArrayData{};
		m_oParseData.m_eType = EDestructuredType::dest_array;
		p = p_bracket_close;
	} else {
		m_oParseData.m_oData = DestructuredObjectData{};
		m_oParseData.m_eType = EDestructuredType::dest_object;
		p = p_curlybracket_close;

	}

	std::advance(m_iterPos, 1);

	return ParseIdentifiers(p);
}
Varjus::Success CDestructuringLinter::ParseIdentifiers(Punctuation untilThisCharacter) {



	auto token = GetIteratorSafe();

	if (IsEndOfBuffer()) {
		m_pOwner->GetModule()->PushError(VSL("expected an identifier"), token->m_oSourcePosition);
		return Varjus::failure;
	}

	const auto isRest = token->IsOperator(p_spread);

	if (isRest) {
		if (GetDestructuredType() != EDestructuredType::dest_array) {
			m_pOwner->GetModule()->PushError(VSL("the rest property (...) is only for array destructuring"), token->m_oSourcePosition);
			return Varjus::failure;
		}
		std::advance(m_iterPos, 1); // skip ...
		token = GetIteratorSafe(); 
	}

	if (IsEndOfBuffer() || !CVariableDeclarationLinter::IsIdentifier(token)) {
		m_pOwner->GetModule()->PushError(VSL("expected an identifier"), token->m_oSourcePosition);
		return Varjus::failure;
	}

	if (const auto var = CVariableDeclarationLinter::DeclareVariable(token, m_pScope, m_pOwner)) {
		var->m_bConst = m_bConst;
		auto&& sharedVar = make_shared<VariableASTNode>(token->m_oSourcePosition, var);
		if (GetDestructuredType() == EDestructuredType::dest_array) {
			if (isRest) {
				std::get<0>(m_oParseData.m_oData).m_pRest = sharedVar;
			} else {
				std::get<0>(m_oParseData.m_oData).m_oProperties.emplace_back(sharedVar);
			}
		} else {
			std::get<1>(m_oParseData.m_oData).emplace_back(DestructuredObjectRef{ 
				sharedVar, 
				m_pOwner->GetProgramInformation()->m_oAllMembers[token->Source()] 
			});
		}
	} else {
		return Varjus::failure;
	}

	std::advance(m_iterPos, 1);

	if (IsEndOfBuffer() || (!(*m_iterPos)->IsOperator(untilThisCharacter) && !(*m_iterPos)->IsOperator(p_comma))) {
		m_pOwner->GetModule()->PushError(VSL("expected a \",\" or the closing symbol"), token->m_oSourcePosition);
		return Varjus::failure;
	}

	if ((*m_iterPos)->IsOperator(p_comma)) {

		if (isRest) {
			m_pOwner->GetModule()->PushError(VSL("the rest property must be the last destructured element"), (*m_iterPos)->m_oSourcePosition);
			return Varjus::failure;
		}

		std::advance(m_iterPos, 1); // skip the comma
		return ParseIdentifiers(untilThisCharacter);
	}

	std::advance(m_iterPos, 1); // skip the end character
	return Varjus::success;

}
bool CDestructuringLinter::IsDestructuringToken(const CToken* token) noexcept {
	return token && (token->IsOperator(p_bracket_open) || token->IsOperator(p_curlybracket_open));
}