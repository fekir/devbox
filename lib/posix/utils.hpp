////
// Copyright (C) 2016 Federico Kircheis
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.
//

/// This package contains utility functions that do not belong to a specific module (yet)


#ifndef H_UTILS_0
#define H_UTILS_0

// local
#include "string_utils.hpp"

#include "posix_memory.hpp"
#include "glib_memory.hpp"
#include "glib_iterator.hpp"

// caja
#include <libcaja-extension/caja-file-info.h>
#include <libcaja-extension/caja-menu.h>

// posix
#include <fnmatch.h> // fnmatch

// cstd
#include <cstring>
#include <cassert>

// std
#include <string>
#include <fstream>
#include <algorithm>
#include <functional>
#include <algorithm>
#include <iterator>

/// return current path if a directory, path to file if file, also handles x-caja-desktop
/// on failure (smb:///, sftp, ...) returns empty string
inline std::string get_path(CajaFileInfo* file_info){
	if(caja_file_info_is_directory(file_info)){
		const std::string desktop("x-caja-desktop:///");
		const gchar_handle uri(caja_file_info_get_uri(file_info));
		const std::size_t len = std::strlen(uri.get());
		if(len >= desktop.size() && desktop.compare(0, desktop.size(), uri.get(), desktop.size()) == 0){
			const char* home = g_get_home_dir();
			return to_string(home);
		}
		const gchar_handle path(g_filename_from_uri(uri.get(), nullptr, nullptr));
		return to_string(path.get());
	}
	const gchar_handle uri(caja_file_info_get_parent_uri(file_info));
	const gchar_handle path(g_filename_from_uri(uri.get(), nullptr, nullptr));
	return to_string(path.get());
}

inline bool file_match(const std::string& pattern, const std::string& file_name){
	int res = fnmatch(pattern.c_str(), file_name.c_str(), 0);
	assert(res == 0 || res == FNM_NOMATCH);
	return res == 0;
}

inline std::string get_name(CajaFileInfo* file_info){
	gchar_handle n(caja_file_info_get_name(file_info));
	std::string name = (n == nullptr) ? "" : n.get();
	return name;
}

const std::string mimetype_exec("application/x-executable");
const std::string mimetype_sharedlib("application/x-sharedlib");
const std::string mimetype_jpeg("image/jpeg");

inline std::string get_mimetype(CajaFileInfo* file_info){
	gchar_handle myme_type(caja_file_info_get_mime_type(file_info));
	return (myme_type == nullptr) ? "" : myme_type.get();
}

struct compare_mimetype{
	std::string mimetype;
	explicit compare_mimetype(const std::string& mime) : mimetype(mime){}
	bool operator()(CajaFileInfo* f){
		return get_mimetype(f)!=mimetype_jpeg;
	}
};

inline std::string to_string(const dirent& dir){
	return to_string(dir.d_name);
}

// searches if at least on of the asked file is present
inline bool dir_contains(const std::string& directory, const std::vector<std::string>& files){
	const DIR_handle dirp(opendir(directory.c_str()));
	if(dirp == nullptr){
		return false;
	}
	dirent* dp = readdir(dirp.get());
	while(dp != nullptr){
		if(std::find(files.begin(), files.end(), to_string(*dp)) != files.end()){
			return true;
		}
		dp = readdir(dirp.get());
	}
	return false;
}

inline bool dir_match(const std::string& directory, const std::vector<std::string>& matcher){
	const DIR_handle dirp(opendir(directory.c_str()));
	if(dirp== nullptr){
		return false;
	}
	dirent* dp = readdir(dirp.get());
	while(dp != nullptr){
		const auto file = to_string(*dp);
		using namespace std::placeholders;
		const auto match_against_filename = std::bind(file_match, _1, file);
		const auto it = std::find_if(matcher.begin(), matcher.end(), match_against_filename);
		if(it != matcher.end()){
			return true;
		}
		dp = readdir(dirp.get());
	}
	return false;
}

const std::vector<std::string> cmake_files = {"CMakeLists.txt", "CMakeFiles", "CMakeCache.txt"};
inline std::vector<std::string> is_cmake_project(const std::vector<CajaFileInfo*>& file_infos){
	if(file_infos.size() != 1){
		return {};
	}
	auto file_info = file_infos.at(0);
	std::vector<std::string> to_return;
	if(!caja_file_info_is_directory(file_info)){
		std::string name = get_name(file_info);
		if(std::find(cmake_files.begin(), cmake_files.end(), name) != cmake_files.end()){
			to_return.push_back(get_path(file_info));
		}
		return to_return;
	}

	auto path = get_path(file_info);
	if(dir_contains(path, cmake_files)){
		to_return.push_back(path);
	}
	return to_return;
}

const std::vector<std::string> c_cpp_files = {"*.cpp", "*.hpp", "*.h", "*.c"};
inline bool is_c_or_cpp_file(CajaFileInfo * f){
	const auto name = get_name(f);
	using namespace std::placeholders;
	const auto match_against_filename = std::bind(file_match, _1, name);
	return std::any_of(c_cpp_files.begin(), c_cpp_files.end(), match_against_filename);
}

inline std::vector<std::string> cppcheck_analyze(const std::vector<CajaFileInfo*>& file_infos){
	if(!std::any_of(file_infos.begin(), file_infos.end(), is_c_or_cpp_file)) {
		return {};
	}
	return {"--enable=all", get_path(file_infos.at(0))};
}


const std::vector<std::string> qt_files = {"CMakeLists.txt", "*.pro"};
inline std::vector<std::string> is_qt_project(const std::vector<CajaFileInfo*>& file_infos){
	if(file_infos.size() != 1){
		return {};
	}
	auto file_info = file_infos.at(0);
	std::vector<std::string> to_return;
	if(!caja_file_info_is_directory(file_info)){
		const std::string name = get_name(file_info);
		if(std::find(cmake_files.begin(), cmake_files.end(), name) != cmake_files.end()){
			to_return.push_back(get_path(file_info));
		}
		return to_return;
	}

	const auto path = get_path(file_info);
	if(dir_match(path, qt_files)){
		to_return.push_back(path);
	}
	return to_return;
}

inline std::vector<std::string> use_jpeg_optim(const std::vector<CajaFileInfo*>& file_infos_){
	if(file_infos_.empty()){
		return {};
	}
	std::vector<CajaFileInfo*> file_infos(file_infos_);
	compare_mimetype comparator(mimetype_jpeg);
	const auto it = std::remove_if(file_infos.begin(), file_infos.end(), comparator);
	file_infos.erase(it,file_infos.end());

	std::vector<std::string> to_return; to_return.reserve(file_infos.size());
	for(const auto& file_info : file_infos){
		to_return.push_back("\"" + get_path(file_info) + "/" + get_name(file_info) + "\"");
	}
	if(to_return.size()>=2){
		to_return.insert(to_return.begin(), "-t");
	}
	return to_return;
}

inline std::vector<std::string> use_valgrind(const std::vector<CajaFileInfo*>& file_infos){
	if(file_infos.size() != 1 || get_mimetype(file_infos.at(0)) != mimetype_exec){
		return {};
	}
	return {get_path(file_infos.at(0)) + "/" + get_name(file_infos.at(0))};
}

/// FIXME: from the return value it's not possible to detect if the variable exists and is empty, or if it does not
///        ideally we should return a std::optional<std::string>
inline std::string get_env(const std::string& var){
	const char* env = std::getenv(var.c_str());
	return to_string(env);
}

// FIXME: it is not secure as execute, but I have not found any way to execute custom binary inside a graphical console
//        it also does not remain in history
inline std::string create_command_for_console(const std::string& program_and_param){
	const std::string newline = "\"\n\"";
	const auto separator = "\"" + std::string(80, '#') + "\n\"";

	std::string command = "/bin/bash -ic 'echo executing \"" + program_and_param + "\";\n";
	command += "printf " + separator + newline + ";";
	command += program_and_param + ";";
	command += "printf " + newline + separator + ";";
	command += "echo finished execution;";
	command += "exec /bin/bash -i'";
	return command;
}

inline std::vector<std::string> get_env_path(){
	const auto path = get_env("PATH");
	return split(path, ':');
}

inline std::string find_executable(const std::vector<std::string>& path, const std::string& filename){
	for(const auto& p : path){
		const std::string name = p + "/" + filename;
		if(access( name.c_str(), X_OK ) != -1 ){
			return name;
		}
	}
	return "";
}


#endif
