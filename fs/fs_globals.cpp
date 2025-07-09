
#include <filesystem>
#include <fstream>
#include <algorithm>
#include "fs_globals.hpp"
#include "fs_io.hpp"

using namespace Varjus;

namespace _fs = std::filesystem;

VarjusString fs::exe_file_name()
{
#ifdef UNICODE
	return LocaleConverter::wstring_to_u16string(std::filesystem::current_path().wstring());
#else
	return std::filesystem::current_path().string();
#endif
}
VarjusString fs::exe_path()
{
#ifdef UNICODE
	return LocaleConverter::wstring_to_u16string(std::filesystem::current_path().wstring());
#else
	return std::filesystem::current_path().string();
#endif
}
VarjusString fs::get_extension(const VarjusString& file)
{
	size_t const extensionPos = file.find_last_of(VSL("."));

	if (extensionPos == VarjusString::npos)
		return VSL("");

	return file.substr(extensionPos);
}
VarjusString fs::previous_directory(const VarjusString& directory)
{
	size_t pos = directory.find_last_of(DIRECTORY_SEPARATOR_CHAR);
	if (pos < 1 || pos == VarjusString::npos)
		return directory;

	return directory.substr(0, pos);
}
VarjusString fs::get_file_name(const VarjusString& fullpath)
{
	size_t pos = fullpath.find_last_of(DIRECTORY_SEPARATOR_CHAR);

	if (pos < 1 || pos == VarjusString::npos)
		return fullpath;

	return fullpath.substr(pos + 1);
}
VarjusString fs::get_file_name_no_extension(const VarjusString& fullpath)
{
	auto file = get_file_name(fullpath);
	auto extension = get_extension(file);
	return file.substr(0, file.size() - extension.size());

}

bool fs::create_directory(const VarjusString& path)
{
	return std::filesystem::create_directory(path);
}

bool fs::valid_file_name(const VarjusString& name)
{
	if (name.empty())
		return false;

	for (const auto& i : name) {
		if (!std::isalnum(i) && i != '-' && i != '_' && i != ' ')
			return false;

	}
	return true;
}
bool fs::directory_exists(const VarjusString& d) { return std::filesystem::exists(d); }
bool fs::file_exists(const VarjusString& f) { return std::filesystem::exists(f); }