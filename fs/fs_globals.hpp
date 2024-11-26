#pragma once

#include <string>
#include <vector>

namespace fs
{


	std::string exe_file_name();
	std::string exe_path();
	std::string get_extension(const std::string& path);
	std::string previous_directory(std::string& directory);
	std::string get_file_name(const std::string& fullpath);
	std::string get_file_name_no_extension(const std::string& fullpath);

	void create_file(const std::string& path);
	bool create_directory(const std::string& path);

	std::vector<std::string> files_in_directory(const std::string& path);
	std::vector<std::string> items_in_directory_formatted(const std::string& path);

	std::string get_last_error();

	bool directory_exists(const std::string& d);
	bool file_exists(const std::string& f);

	bool valid_file_name(const std::string& name);

}
