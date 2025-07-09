#pragma once

#include <memory>

#include "varjus_api/internal/globalDefinitions.hpp"
#include "declarations/members.hpp"
#include "error.hpp"

struct CFileContext
{
	CFileContext() = default;
	CFileContext(const VarjusString& wd) : m_sFilePath(wd){}

	VarjusString m_sFilePath;

	CStringIntegerHashMap m_oAllVariables;
	CStringIntegerHashMap m_oAllGlobalVariables;

	void Reset() noexcept {
		m_sFilePath.clear();
		m_oAllVariables.Clear();
		m_oAllGlobalVariables.Clear();
	}

};
class CProjectModules;

namespace Varjus {
	class CBuiltInFunctions;
	class CBuiltInObjects;

	class CProgramInformation
	{
		VARJUS_NONCOPYABLE(CProgramInformation);
	public:
		CProgramInformation();
		~CProgramInformation();

		[[nodiscard]] inline auto GetModules() const { return m_pModules.get(); }


		[[nodiscard]] inline auto PushError(const VarjusString& err) { m_oErrors.PushError(VSL(""), err); }
		[[nodiscard]] inline auto PushError(const VarjusString& err, const __CodePosition& pos) {
			m_oErrors.PushError(VSL(""), err, pos);
		}

		CStringIntegerHashMap m_oAllMembers;
		std::unique_ptr<CProjectModules> m_pModules;
		std::unique_ptr<Varjus::CBuiltInObjects> m_oBuiltInObjects; //declared by the user
		std::unique_ptr<Varjus::CBuiltInFunctions> m_oBuiltInFunctions; //declared by the user

		CLinterErrors m_oErrors; //global level errors
	};
}