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
#include <iterator>


// array with common spaces, used by trim functions. A lot of UTF characters are missing.
constexpr char char_to_trim[] = { ' ', '\0', '\t', '\n', '\r', '\v', '\f'};

template < class T, size_t N >
constexpr size_t array_size( const T (&)[N] ) noexcept{return N;}


inline std::string rtrim(const std::string& s){
	const auto last = s.find_last_not_of(char_to_trim, std::string::npos, array_size(char_to_trim));
	return s.substr(0, last);
}

inline std::string ltrim(const std::string& s){
	const auto first = s.find_first_not_of(char_to_trim, 0, array_size(char_to_trim));
	return s.substr(first);
}

inline std::string trim(const std::string& s){
	// not calling ltrim and rtrim for avoiding one reallocation
	const auto last = s.find_last_not_of(char_to_trim, std::string::npos, array_size(char_to_trim));
	const auto first = s.find_first_not_of(char_to_trim, 0, array_size(char_to_trim));
	return s.substr(first, last-first+1);
}


/// reads a whole line inside a std::string
inline bool mygetline(std::string& l, FILE* fp){
	if(!feof(fp)){
		l={};
		return false;
	}
	std::string buffer;
	while(!feof(fp)) {
		std::string tmp(256, '\0');
		if(fgets(&tmp[0], static_cast<int>(tmp.size()), fp) != nullptr){
			const auto len = std::strlen(tmp.c_str());
			if ((len > 0) && (tmp.at(len-1) == '\n')){ // read untile end of line (what if line ends with '\r'?
				buffer += std::string(tmp.c_str(), len -2);
				l = buffer;
				return true;
			} else if(len > 0) { // but no terminating line
				buffer += std::string(tmp.c_str(), len);
			}
		}
	}
	// FIXME: add error checking
	// finished reading file, no eol, but eof --> ok
	l = buffer;
	return true;
}

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

const std::string mimetype_exec("application/x-executable");
const std::string mimetype_sharedlib("application/x-sharedlib");
const std::string mimetype_jpeg("image/jpeg");

inline std::string get_mimetype(CajaFileInfo* file_info){
	gchar_handle myme_type(caja_file_info_get_mime_type (file_info));
	return (myme_type == nullptr) ? "" : myme_type.get();
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

inline bool dir_match(const std::string& directory, const std::vector<std::string>& matcher){
	const DIR_handle dirp(opendir(directory.c_str()));
	if(dirp== nullptr){
		return false;
	}
	dirent* dp = readdir(dirp.get());
	while(dp != nullptr){
		const std::string file(dp->d_name ? dp->d_name : "");
		const auto it = std::find_if(matcher.begin(), matcher.end(),
									 [&file](const std::string& m){return fnmatch(m.c_str(), file.c_str(), 0) == 0;}
		);
		if( it != matcher.end()){
			return true;
		}
		dp = readdir(dirp.get());
	}
	return false;
}


const std::vector<std::string> cmake_files = {"CMakeLists.txt", "CMakeFiles", "CMakeCache.txt"};
inline std::vector<std::string> is_cmake_project(const std::vector<CajaFileInfo*> file_infos){
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
inline std::vector<std::string> cppcheck_analyze(const std::vector<CajaFileInfo*>& file_infos){
	const auto lambda1 =[](CajaFileInfo* f){
		const auto name = get_name(f);
		return std::any_of(c_cpp_files.begin(), c_cpp_files.end(), [&name](const std::string& file_type){
			return fnmatch(file_type.c_str(), name.c_str(), 0) ==0;
		});
	};
	if(!std::any_of(file_infos.begin(), file_infos.end(),lambda1)){
		return {};
	}
	return { "--enable=all", get_path(file_infos.at(0))};
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

inline std::vector<std::string> use_jpeg_optim(const std::vector<CajaFileInfo*> file_infos_){
	if(file_infos_.empty()){
		return {};
	}
	std::vector<CajaFileInfo*> file_infos(file_infos_);
	const auto it = std::remove_if(file_infos.begin(), file_infos.end(),
								   [](CajaFileInfo* f){return get_mimetype(f)!=mimetype_jpeg;}
			);
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

inline std::vector<std::string> use_valgrind(const std::vector<CajaFileInfo*> file_infos){
	if(file_infos.size() != 1 || get_mimetype(file_infos.at(0)) != mimetype_exec){
		return{};
	}
	return {get_path(file_infos.at(0)) + "/" + get_name(file_infos.at(0))};
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
