#include "members.hpp"

std::size_t CMembers::operator[](const std::string& key)
{

	const auto index = m_oStringToId.find(key);

	if (index == m_oStringToId.end()) {
		m_oStringToId[key] = GetCurrentIndex();
		m_oIdToString[m_oStringToId[key]] = key;
	}

	return m_oStringToId[key];
}
bool CMembers::Contains(const std::string& key) const
{
	return m_oStringToId.contains(key);
}

std::size_t CMembers::GetCurrentIndex() const
{
	 return m_oStringToId.size();
}
