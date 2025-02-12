#pragma once

#include "declarations/members.hpp"

struct CFileContext
{
	CFileContext() { m_oAllMembers.Insert("length"); }
	CFileContext(const std::string& wd) : m_sFilePath(wd){
		m_oAllMembers.Insert("length");
	}

	std::string m_sFilePath;

	static CStringIntegerHashMap m_oAllMembers; //cross module...
	CStringIntegerHashMap m_oAllVariables;
	CStringIntegerHashMap m_oAllGlobalVariables;
	
	static void ResetGlobally() noexcept {
		m_oAllMembers.Clear();
	}

	void Reset() noexcept {
		m_sFilePath.clear();
		m_oAllVariables.Clear();
		m_oAllGlobalVariables.Clear();
	}

};
