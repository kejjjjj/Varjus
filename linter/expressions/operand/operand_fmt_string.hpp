#pragma once

#include "operand.hpp"
#include "linter/token.hpp"

#include <variant>


struct FmtStringAST;

class CFmtStringToken : public CToken
{
public:
	enum FmtStringTokenType { raw, placeholder };

	CFmtStringToken()  : CToken(VSL("internal formatString"), tt_fmt_string) { }
	constexpr void InsertFmtToken(std::unique_ptr<CToken>&& token, FmtStringTokenType t) { 
		m_oTokens.emplace_back(std::move(token));
		m_oTokenTypes.push_back(t);
	}

	[[nodiscard]] constexpr auto GetTokens() const noexcept { 
		VectorOf<CToken*> ptrs;

		for (auto& ptr : m_oTokens)
			ptrs.push_back(ptr.get());
	
		return ptrs;
	}
	[[nodiscard]] constexpr auto& GetTokenTypes() const noexcept { return m_oTokenTypes; }

private:
	//should always have the same size
	VectorOf<std::unique_ptr<CToken>> m_oTokens;
	VectorOf<FmtStringTokenType> m_oTokenTypes;
};

struct CFmtStringOperand final : public IOperand
{

	VARJUS_NONCOPYABLE(CFmtStringOperand);

	CFmtStringOperand() = default;
	CFmtStringOperand(VectorOf<FmtStringAST>&& nodes);
	~CFmtStringOperand();

	[[nodiscard]] EOperandBaseType Type() const noexcept override {
		return ot_fmt_string;
	}

	[[nodiscard]] ASTNode ToAST() override;


private:
	VectorOf<FmtStringAST> m_oNodes;
};
