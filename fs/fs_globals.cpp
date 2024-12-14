
#include <filesystem>
#include <fstream>
#include "fs_globals.hpp"

namespace _fs = std::filesystem;

std::string fs::exe_file_name()
{
	return std::filesystem::current_path().string();
}
std::string fs::exe_path()
{
	return std::filesystem::current_path().string();
}
std::string fs::get_extension(const std::string& file)
{
	size_t const extensionPos = file.find_last_of(".");

	if (extensionPos == std::string::npos)
		return "";

	return file.substr(extensionPos);
}
std::string fs::previous_directory(const std::string& directory)
{
	size_t pos = directory.find_last_of('\\');
	if (pos < 1 || pos == std::string::npos)
		return directory;

	return directory.substr(0, pos);
}
std::string fs::get_file_name(const std::string& fullpath)
{
	size_t pos = fullpath.find_last_of('\\');

	if (pos < 1 || pos == std::string::npos)
		return fullpath;

	return fullpath.substr(pos + 1);
}
std::string fs::get_file_name_no_extension(const std::string& fullpath)
{
	auto file = get_file_name(fullpath);
	auto extension = get_extension(file);
	return file.substr(0, file.size() - extension.size());

}

void fs::create_file(const std::string& path)
{
	std::fstream* nf = new std::fstream(path, std::ios_base::out);
	*nf << "";
	if (nf->is_open())
		nf->close();
	delete nf;
}
bool fs::create_directory(const std::string& path)
{
	return std::filesystem::create_directory(path);
}
std::vector<std::string> fs::files_in_directory(const std::string& path)
{
	std::vector<std::string> files;

	if (!_fs::exists(path)) {
		return {};
	}

	for (const auto& entry : _fs::directory_iterator(path)) {
		if (entry.is_directory())
			continue;

		std::string str = entry.path().string();
		files.push_back(std::move(str));
	}

	return (files); //compiler I hope you optimize this! 
}

std::vector<std::string> fs::items_in_directory_formatted(const std::string& path)
{
	std::vector<std::string> files;

	if (!_fs::exists(path)) {
		return {};
	}

	for (const auto& entry : _fs::directory_iterator(path)) {
		const auto name = fs::get_file_name(entry.path().string());

		std::string prefix = entry.is_directory() ? "(folder) " : "(file) ";
		std::string str = prefix + name;
		files.push_back(str);
	}

	std::sort(files.begin(), files.end());

	return files; //compiler I hope you optimize this! 
}

bool fs::valid_file_name(const std::string& name)
{
	if (name.empty())
		return false;

	for (const auto& i : name) {
		if (!std::isalnum(i) && i != '-' && i != '_' && i != ' ')
			return false;

	}
	return true;
}
bool fs::directory_exists(const std::string& d) { return std::filesystem::exists(d); }
bool fs::file_exists(const std::string& f) { return std::filesystem::exists(f); }