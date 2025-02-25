#pragma once

#include "api/internal/globalDefinitions.hpp"
#include "declarations/members.hpp"
#include "error.hpp"

struct CFileContext
{
	CFileContext() = default;
	CFileContext(const std::string& wd) : m_sFilePath(wd){}

	std::string m_sFilePath;

	CStringIntegerHashMap m_oAllVariables;
	CStringIntegerHashMap m_oAllGlobalVariables;

	void Reset() noexcept {
		m_sFilePath.clear();
		m_oAllVariables.Clear();
		m_oAllGlobalVariables.Clear();
	}

};
class CProjectModules;
class CBuiltInObjects;
class CBuiltInFunctions;
class CProgramInformation
{
	NONCOPYABLE(CProgramInformation);
public:
	CProgramInformation();
	~CProgramInformation();

	[[nodiscard]] constexpr auto GetModules() const { return m_pModules.get(); }


	[[nodiscard]] inline auto PushError(const std::string& err) noexcept { m_oErrors.PushError("", err); }
	[[nodiscard]] inline auto PushError(const std::string& err, const CodePosition& pos) noexcept {
		m_oErrors.PushError("", err, pos);
	}

	CStringIntegerHashMap m_oAllMembers;
	std::unique_ptr<CProjectModules> m_pModules;
	std::unique_ptr<CBuiltInObjects> m_oBuiltInObjects; //declared by the user
	std::unique_ptr<CBuiltInFunctions> m_oBuiltInFunctions; //declared by the user

	CLinterErrors m_oErrors; //global level errors
};
