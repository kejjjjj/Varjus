#pragma once


#include <vector>

#include "varjus_api/internal/globalDefinitions.hpp"

namespace Varjus {
	namespace fs {
		VarjusString exe_file_name();
		VarjusString exe_path();
		VarjusString get_extension(const VarjusString& path);
		VarjusString previous_directory(const VarjusString& directory);
		VarjusString get_file_name(const VarjusString& fullpath);
		VarjusString get_file_name_no_extension(const VarjusString& fullpath);

		bool create_directory(const VarjusString& path);

		bool directory_exists(const VarjusString& d);
		bool file_exists(const VarjusString& f);

		bool valid_file_name(const VarjusString& name);

	}
}