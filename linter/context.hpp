#pragma once

#include "declarations/members.hpp"

struct CFileContext
{
	CFileContext() { m_oAllMembers.Insert("length"); }
	CFileContext(const std::string& wd) : m_sFilePath(wd){
		m_oAllMembers.Insert("length");
	}

	std::string m_sFilePath;

	CStringIntegerHashMap m_oAllMembers;
	CStringIntegerHashMap m_oAllVariables;
	CStringIntegerHashMap m_oAllGlobalVariables;

	void Reset() {
		m_sFilePath.clear();
		m_oAllMembers.Clear();
		m_oAllVariables.Clear();
		m_oAllGlobalVariables.Clear();
	}

};
