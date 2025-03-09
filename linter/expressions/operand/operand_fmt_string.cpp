#include "operand_fmt_string.hpp"
#include "operand.hpp"

#include "linter/expressions/ast.hpp"
#include "linter/expressions/expression.hpp"
#include "linter/error.hpp"
#include "linter/token.hpp"
#include "linter/tokenizer.hpp"
#include "linter/punctuation.hpp"

#include <ranges>
#include <cassert>



CFmtStringOperand::CFmtStringOperand(VectorOf<FmtStringAST>&& nodes)
	: m_oNodes(std::move(nodes)){}

CFmtStringOperand::~CFmtStringOperand() = default;

std::unique_ptr<IOperand> CLinterOperand::ParseFormatString()
{
	const auto& parseString = dynamic_cast<CFmtStringToken*>(*m_iterPos);
	auto oldIter = m_iterPos;

	VectorOf<FmtStringAST> fmtString;

	auto tokens = parseString->GetTokens();
	auto& tokenTypes = parseString->GetTokenTypes();

	VarjusString str;
	auto end = tokens.end();

	for (auto it = tokens.begin(); it != end;) {

		auto i = static_cast<std::size_t>(std::distance(tokens.begin(), it));
		auto& token = *it;

		if (tokenTypes[i] == CFmtStringToken::FmtStringTokenType::raw) {
			str += token->Source();
			it++;
			continue;
		}

		if (str.length()) {
			fmtString.emplace_back(FmtStringAST(str));
			str.clear();
		}

		CLinterExpression expr(it, end, m_pScope, m_pOwner);

		if (!expr.Parse(PairMatcher(p_curlybracket_open)))
			return nullptr;

		fmtString.emplace_back(FmtStringAST(expr.ToMergedAST()));

	}

	if (str.length()) {
		fmtString.emplace_back(FmtStringAST(str));
		str.clear();
	}

	std::advance(m_iterPos, 1);
	auto ptr = std::make_unique<CFmtStringOperand>(std::move(fmtString));
	ptr->m_oCodePosition = (*oldIter)->m_oSourcePosition;
	return ptr;
}

ASTNode CFmtStringOperand::ToAST() {
	return std::make_shared<FmtStringASTNode>(m_oCodePosition, std::move(m_oNodes));
}
