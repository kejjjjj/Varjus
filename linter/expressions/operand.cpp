#include "operand.hpp"
#include "unary.hpp"
#include "identifier.hpp"
#include "postfix.hpp"
#include "expression.hpp"

#include "linter/punctuation.hpp"
#include "linter/token.hpp"
#include "linter/error.hpp"
#include "linter/context.hpp"
#include "linter/functions/stack.hpp"

#include "ast.hpp"

#include "globalEnums.hpp"

#include <cassert>
#include <ranges>
#include <iostream>

CIdentifierLinter* COperandBase::GetIdentifier()const  noexcept {
	return dynamic_cast<const CIdentifierOperand*>(this)->m_oIdentifierToken.get();
}


CLinterOperand::CLinterOperand(LinterIterator& pos, LinterIterator& end, const WeakScope& scope, 
	CMemory* const stack, std::optional<PairMatcher>& eoe)
	:CLinterSingle(pos, end), m_pScope(scope), m_pOwner(stack), m_oEndOfExpression(eoe) {

	assert(m_iterPos != m_iterEnd);

}
CLinterOperand::~CLinterOperand() = default;

Success CLinterOperand::ParseOperand()
{
	// Parse unary
	CUnaryLinter unaryLinter(m_iterPos, m_iterEnd);
	if (!unaryLinter.ParseUnary())
		return failure;


	const auto token = (*m_iterPos);
	if (token->IsOperator(p_par_open)) {
		m_pOperand = ParseParentheses();
	} else if (token->IsOperator(p_bracket_open)) {
		m_pOperand = ParseArray();
	} else if (token->IsOperator(p_curlybracket_open)) {
		m_pOperand = ParseObject();
	} else {
		m_pOperand = ParseIdentifier();
	}

	CPostfixLinter postfix(m_iterPos, m_iterEnd, m_pScope, m_pOwner);
	if (!postfix.ParsePostfix())
		return failure;

	// Save results
	m_oUnaryTokens = unaryLinter.GetTokens();
	m_oPostfixes = postfix.Move();
	return success;
}
std::unique_ptr<COperandBase> CLinterOperand::ParseParentheses()
{
	std::advance(m_iterPos, 1); // skip (

	CLinterExpression expr(m_iterPos, m_iterEnd, m_pScope, m_pOwner);
	if (!expr.Parse(PairMatcher(p_par_open))) 
		return nullptr;

	return std::make_unique<CASTOperand>(expr.ToMergedAST());
}
std::unique_ptr<COperandBase> CLinterOperand::ParseIdentifier()
{
	//key:value?
	auto nextToken = std::next(m_iterPos);
	if (!EndOfExpression(m_oEndOfExpression, nextToken)) {
		if ((*nextToken)->IsOperator(p_colon))
			return ParseKeyValue();
	}

	auto operand = std::make_unique<CIdentifierOperand>(m_iterPos, m_iterEnd, m_pScope, m_pOwner);
	if (!operand->m_oIdentifierToken->ParseIdentifier()) {
		return nullptr;
	}

	return operand;
}
std::unique_ptr<COperandBase> CLinterOperand::ParseArray()
{
	std::advance(m_iterPos, 1); // skip [

	if(IsEndOfBuffer() || (*m_iterPos)->IsOperator(p_bracket_close))
		return std::make_unique<CArrayOperand>();

	CLinterExpression expr(m_iterPos, m_iterEnd, m_pScope, m_pOwner);
	if (!expr.Parse(PairMatcher(p_bracket_open)))
		return nullptr;

	return std::make_unique<CArrayOperand>(expr.ToExpressionList());
}
std::unique_ptr<COperandBase> CLinterOperand::ParseKeyValue()
{
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
	if (!expr.ParseInternal(m_oEndOfExpression))
		return nullptr;

	return std::make_unique<CKeyValueOperand>(
		std::make_pair(
			m_pOwner->GetContext()->m_oAllMembers[identifier],
			expr.ToMergedAST()
		)
	);
}
std::unique_ptr<COperandBase> CLinterOperand::ParseObject()
{
	std::advance(m_iterPos, 1); // skip {

	if (IsEndOfBuffer() || (*m_iterPos)->IsOperator(p_curlybracket_close))
		return std::make_unique<CObjectOperand>();

	CLinterExpression expr(m_iterPos, m_iterEnd, m_pScope, m_pOwner);
	if (!expr.Parse(PairMatcher(p_curlybracket_open)))
		return nullptr;

	auto object = std::make_unique<CObjectOperand>();
	for (auto& elem : expr.ToExpressionList()) {

		auto keyValue = const_cast<KeyValueASTNode*>(elem->GetKeyValue());

		if (!keyValue) {
			CLinterErrors::PushError("expected a key:value pair in object creation", GetIteratorSafe()->m_oSourcePosition);
			return nullptr;
		}

		object->m_oAttributes.emplace_back(std::move(keyValue->m_oValue));
	}

	return object;

}

bool CLinterOperand::IsExpression() const noexcept
{
	assert(m_pOperand != nullptr);
	return m_pOperand->Type() == ot_abstract_syntax_tree;
}
bool CLinterOperand::IsArray() const noexcept
{
	assert(m_pOperand != nullptr);
	return m_pOperand->Type() == ot_array;
}
[[nodiscard]] CArrayOperand* CLinterOperand::GetArray() const noexcept
{
	assert(IsArray());
	return dynamic_cast<CArrayOperand*>(m_pOperand.get());
}
bool CLinterOperand::IsKeyValue() const noexcept
{
	assert(m_pOperand != nullptr);
	return m_pOperand->Type() == ot_key_value;
}
CKeyValueOperand* CLinterOperand::GetKeyValue() const noexcept
{
	assert(IsKeyValue());
	return dynamic_cast<CKeyValueOperand*>(m_pOperand.get());
}
bool CLinterOperand::IsObject() const noexcept
{
	assert(m_pOperand != nullptr);
	return m_pOperand->Type() == ot_object;
}
CObjectOperand* CLinterOperand::GetObject() const noexcept
{
	assert(IsObject());
	return dynamic_cast<CObjectOperand*>(m_pOperand.get());
}

std::unique_ptr<AbstractSyntaxTree> CLinterOperand::OperandToAST() const noexcept
{
	if (IsImmediate()) {
		const auto identifier = m_pOperand->GetIdentifier();
		return std::make_unique<ConstantASTNode>(identifier->ToData(), identifier->GetImmediateType());
	} else if (IsVariable()) {
		return std::make_unique<VariableASTNode>(GetVariable()->m_uIndex);
	} else if (IsFunction()) {
		return std::make_unique<FunctionASTNode>(GetFunction()->m_uIndex);
	} else if (IsArray()) {
		return std::make_unique<ArrayASTNode>(std::move(GetArray()->m_oExpressions));
	} else if (IsKeyValue()) {
		return std::make_unique<KeyValueASTNode>(std::move(GetKeyValue()->m_oValue));
	} else if (IsObject()) {
		return std::make_unique<ObjectASTNode>(std::move(GetObject()->m_oAttributes));
	} else if (IsExpression()) {
		return ExpressionToAST();
	}

	return nullptr;
}
std::unique_ptr<AbstractSyntaxTree> CLinterOperand::ExpressionToAST() const noexcept
{
	assert(m_pOperand != nullptr);
	assert(IsExpression());
	return std::move(dynamic_cast<CASTOperand*>(m_pOperand.get())->m_pAST);
}

[[nodiscard]] std::unique_ptr<AbstractSyntaxTree> CLinterOperand::ToAST()
{
	if (auto pfs = PostfixesToAST()) {
		auto end = pfs.get();
		while (end->left)
			end = end->left.get();

		end->left = OperandToAST();
		return pfs;
	}

	return OperandToAST();
}

std::unique_ptr<AbstractSyntaxTree> CLinterOperand::PostfixesToAST() const noexcept
{
	if (!m_oPostfixes.size())
		return nullptr;

	std::unique_ptr<AbstractSyntaxTree> root;
	AbstractSyntaxTree* position{ nullptr };

	for (auto& pf : m_oPostfixes) {
		
		if (!root) {
			root = pf->ToAST();
			position = root.get();
			continue;
		}

		assert(position);

		position->left = pf->ToAST();
		position = position->left.get();
	}

	return root;
}
bool CLinterOperand::IsImmediate() const noexcept
{
	assert(m_pOperand != nullptr);

	if (IsExpression() || IsArray() || IsObject() || IsKeyValue())
		return false;

	const auto type = m_pOperand->GetIdentifier()->GetToken()->Type();
	return IS_IMMEDIATE(type);
}
bool CLinterOperand::IsVariable() const noexcept
{
	assert(m_pOperand != nullptr);

	if (IsExpression() || IsArray() || IsObject() || IsKeyValue() || !m_pOperand->GetIdentifier()->m_pIdentifier)
		return false;

	return m_pOperand->GetIdentifier()->m_pIdentifier->Type() == mi_variable;
}
const CLinterVariable* CLinterOperand::GetVariable() const noexcept
{
	assert(IsVariable());
	return dynamic_cast<const CLinterVariable*>(m_pOperand->GetIdentifier()->m_pIdentifier);
}
bool CLinterOperand::IsFunction() const noexcept 
{
	assert(m_pOperand != nullptr);

	if (IsExpression() || IsArray() || IsObject() || IsKeyValue() || !m_pOperand->GetIdentifier()->m_pIdentifier)
		return false;

	return m_pOperand->GetIdentifier()->m_pIdentifier->Type() == mi_function;
}
const CLinterFunction* CLinterOperand::GetFunction() const noexcept 
{
	assert(IsFunction());
	return dynamic_cast<const CLinterFunction*>(m_pOperand->GetIdentifier()->m_pIdentifier);
}

bool CLinterOperand::EndOfExpression(const std::optional<PairMatcher>& eoe, LinterIterator& pos) const noexcept
{
	if (pos == m_iterEnd)
		return true;

	if (!eoe)
		return (*pos)->IsOperator(p_semicolon);

	if (!(*pos)->IsOperator())
		return false;

	return eoe->IsClosing(dynamic_cast<const CPunctuationToken*>(*pos)->m_ePunctuation);
}

/***********************************************************************
 > 
***********************************************************************/
CASTOperand::CASTOperand(std::unique_ptr<AbstractSyntaxTree>&& ptr) : m_pAST(std::move(ptr)) {}
CASTOperand::~CASTOperand() = default;

CArrayOperand::CArrayOperand(ExpressionList&& ptr) : m_oExpressions(std::move(ptr)){}
CArrayOperand::~CArrayOperand() = default;

CKeyValueOperand::CKeyValueOperand(KeyValue<std::size_t, UniqueAST>&& ptr) : m_oValue(std::move(ptr)){}
CKeyValueOperand::~CKeyValueOperand() = default;

CObjectOperand::CObjectOperand(VectorOf<KeyValue<std::size_t, UniqueAST>>&& ptr) : m_oAttributes(std::move(ptr)){}
CObjectOperand::~CObjectOperand() = default;
