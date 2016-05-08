/*
	Copyright (C) 2016 Federico Kircheis

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

/// This package contains utility functions that do not belong to a specific module (yet)


#ifndef H_UTILS_0
#define H_UTILS_0

// local
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

// std
#include <string>
#include <fstream>
#include <algorithm>
#include <functional>

/// return current path if a directory, path to file if file, also handles x-caja-desktop
/// on failure (smb:///, sftp, ...) returns empty string
inline std::string get_path(CajaFileInfo* file_info){
	if(caja_file_info_is_directory(file_info)){
		const std::string desktop("x-caja-desktop:///");
		const gchar_handle uri(caja_file_info_get_uri(file_info));
		std::size_t len = std::strlen(uri.get());
		if(len >= desktop.size() && desktop.compare(0, desktop.size(), uri.get(), desktop.size()) == 0){
			const char* home = g_get_home_dir();
			return home == nullptr ? "" : home;
		}
		const gchar_handle path(g_filename_from_uri (uri.get(), nullptr, nullptr));
		return path == nullptr ? "" : path.get();
	}
	const gchar_handle uri(caja_file_info_get_parent_uri (file_info));
	const gchar_handle path(g_filename_from_uri (uri.get(), nullptr, nullptr));
	return path == nullptr ? "" : path.get();
}

/// return current path if a directory, path to file if file, also handles x-caja-desktop
/// a path is a directory if it ends wit '/' (linux)
inline std::string get_path(const std::string& file_info){
	const size_t last_slash = file_info.find_last_of("/");
	if (std::string::npos == last_slash) {
		return "";
	}
	return file_info.substr(0, last_slash + 1);
}

/// return name of file
/// removes path if present
inline std::string get_name(std::string file_info){
	const size_t last_slash = file_info.find_last_of("/");
	if (std::string::npos != last_slash) {
		file_info.erase(0, last_slash + 1);
	}
	return file_info;
}

inline std::string get_name(CajaFileInfo* file_info){
	gchar_handle n (caja_file_info_get_name(file_info));
	std::string name =  (n == nullptr) ? "" : n.get();
	return name;
}

// searches if at least on of the asked file is present
inline bool dir_contains(const std::string& directory, const std::vector<std::string>& files){
	const DIR_handle dirp(opendir(directory.c_str()));
	if(dirp== nullptr){
		return false;
	}
	dirent* dp = readdir(dirp.get());
	while(dp != nullptr){
		if(std::find(files.begin(), files.end(), std::string(dp->d_name ? dp->d_name : "")) != files.end()){
			return true;
		}
		dp = readdir(dirp.get());
	}
	return false;
}


const std::vector<std::string> cmake_files = {"CMakeLists.txt", "CMakeFiles", "CMakeCache.txt"};
inline std::vector<std::string> is_cmake_project_(const std::vector<CajaFileInfo*> file_infos){
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

const std::vector<std::string> qt_files = {"CMakeLists.txt", "*.pro"};

inline std::vector<std::string> is_qt_project_(const std::vector<CajaFileInfo*>& file_infos){
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

	auto path = get_path(file_info);
	if(dir_contains(path, qt_files)){
		to_return.push_back(path);
	}
	return to_return;
}

inline bool is_jpeg(CajaFileInfo* file_info){
	gchar_handle myme_type(caja_file_info_get_mime_type (file_info));
	if(myme_type==nullptr){
		return false;
	}
	return myme_type.get() == std::string("image/jpeg");
}

inline std::vector<std::string> use_jpeg_optim(const std::vector<CajaFileInfo*> file_infos_){
	if(file_infos_.empty()){
		return {};
	}
	std::vector<CajaFileInfo*> file_infos(file_infos_);
	const auto it = std::remove_if(file_infos.begin(), file_infos.end(), [](CajaFileInfo* f){return !is_jpeg(f);});
	file_infos.erase(it,file_infos.end());

	std::vector<std::string> to_return; to_return.reserve(file_infos.size());
	for(const auto& file_info : file_infos){
		to_return.push_back(get_path(file_info) + "/" + get_name(file_info));
	}
	if(to_return.size()>=2){
		to_return.insert(to_return.begin(), "-t");
	}
	return to_return;
}

inline std::string get_env(const std::string& var){
	const char* env = std::getenv(var.c_str());
	return env == nullptr ? "" : env;
}

inline std::string create_command_for_console(const std::string& program_and_param){
	const std::string newline = "\"\n\"";
	const auto separator = "\"" +std::string(80, '#') + "\n\"";

	std::string command = "/bin/bash -ic 'echo executing \"" + program_and_param + "\";\n";
	command += "printf " + separator + newline + ";";
	command += program_and_param + ";";
	command += "printf " + newline + separator + ";";
	command += "echo finished execution;";
	command += "exec /bin/bash -i'";
	return command;
}
#endif
