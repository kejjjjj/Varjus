#include <unordered_map>
#include <string>


using GlobalMemberIndex = std::size_t;
using ElementIndex = std::size_t;

/***********************************************************************
 > provides quick lookup values for object member names
***********************************************************************/
class CStringIntegerHashMap
{
public:
	[[maybe_unused]] std::size_t operator[](const std::string& key);

	[[nodiscard]] bool Contains(const std::string& key) const;
	[[nodiscard]] const std::string& At(const GlobalMemberIndex key) const;
	[[nodiscard]] GlobalMemberIndex At(const std::string& key) const;

	[[nodiscard]] auto& StringToIntIterator() const noexcept { return m_oStringToId; }

	void Clear() { m_oStringToId.clear(); m_oIdToString.clear(); }

private:
	[[nodiscard]] GlobalMemberIndex GetCurrentIndex() const;

	std::unordered_map<std::string, GlobalMemberIndex> m_oStringToId;
	std::unordered_map<GlobalMemberIndex, std::string> m_oIdToString;
};
