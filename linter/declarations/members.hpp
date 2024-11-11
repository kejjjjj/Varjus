#include <unordered_map>
#include <string>

/***********************************************************************
 > provides quick lookup values for object member names
***********************************************************************/
class CMembers
{
public:
	[[maybe_unused]] std::size_t operator[](const std::string& key);

	[[nodiscard]] bool Contains(const std::string& key) const;

private:
	[[nodiscard]] std::size_t GetCurrentIndex() const;

	std::unordered_map<std::string, std::size_t> m_oStringToId;
	std::unordered_map<std::size_t, std::string> m_oIdToString;
};
