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

/// return file in the path
/// a path is a directory if it ends wit '/' (linux)
inline std::string get_name(std::string file_info){
	const size_t last_slash = file_info.find_last_of("/");
	if (std::string::npos != last_slash) {
		file_info.erase(0, last_slash + 1);
	}
	return file_info;
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
inline bool is_cmake_project_(CajaFileInfo *file_info){
	if(!caja_file_info_is_directory(file_info)){
		gchar_handle n (caja_file_info_get_name(file_info));
		std::string name =  (n == nullptr) ? "" : n.get();
		return std::find(cmake_files.begin(), cmake_files.end(), name) != cmake_files.end();
	}

	auto path = get_path(file_info);
	return dir_contains(path, cmake_files);
}


inline std::string is_cmake_project(const std::vector<std::string>& files){
	for(const auto& file : files){
		if(std::find(cmake_files.begin(), cmake_files.end(), file) != cmake_files.end()){
			return "\"" + get_path(file) + "\"";
		}
	}
	return "";
}

inline void cmake_callback(CajaMenuItem* item, gpointer file_){
	(void)item;
	auto file = reinterpret_cast<CajaFileInfo*>(file_);
	std::string program = "/usr/bin/cmake-gui";
	std::string param = get_path(file);
	char* const args[] = {&program[0], &param[0],  nullptr};
	const pid_t child_pid = fork();
	if (child_pid == 0) {  // in child
		execve(args[0], args, environ); // check != -1
	}
}

inline void qt_callback(CajaMenuItem* item, gpointer file_){
	(void)item;
	auto file = reinterpret_cast<CajaFileInfo*>(file_);
	std::string program = "/usr/bin/qtcreator";
	std::string param = get_path(file);
	char* const args[] = {&program[0], &param[0],  nullptr};
	const pid_t child_pid = fork();
	if (child_pid == 0) {  // in child
		execve(args[0], args, environ); // check != -1
	}
}

inline void jpegoptim_callback(CajaMenuItem* item, gpointer file_){
	(void)item;
	auto file = reinterpret_cast<CajaFileInfo*>(file_);
	std::string program = "/usr/bin/jpegoptim";
	std::string param = get_path(file);
	char* const args[] = {&program[0], &param[0],  nullptr};
	const pid_t child_pid = fork();
	if (child_pid == 0) {  // in child
		execve(args[0], args, environ); // check != -1
	}
}
/**/
inline void create_cmake_menu(CajaMenu *menu_root, CajaFileInfo* file_info){
	if(!caja_file_info_is_directory(file_info)){
		gchar_handle n (caja_file_info_get_name(file_info));
		std::string name =  (n == nullptr) ? "" : n.get();
		if(std::find(cmake_files.begin(), cmake_files.end(), name) == cmake_files.end()){
			return;
		}
	}
	auto path = get_path(file_info);
	if(!dir_contains(path, cmake_files)){
		return;
	}
	auto menu = caja_menu_item_new("cmake-gui", "Open project with cmake-gui", "Open project with cmake-gui", nullptr);
	caja_menu_append_item(menu_root, menu);
	g_signal_connect_object (menu, "activate",
							 G_CALLBACK (cmake_callback),
							 file_info, static_cast<GConnectFlags>(0));
}

const std::vector<std::string> qt_files = {"CMakeLists.txt", "*.pro"};
inline void create_qt_menu(CajaMenu *menu_root, CajaFileInfo *file_info){
	if(!caja_file_info_is_directory(file_info)){
		gchar_handle n (caja_file_info_get_name(file_info));
		std::string name =  (n == nullptr) ? "" : n.get();
		auto res = std::find_if(qt_files.begin(), qt_files.end(),
								[&name](const std::string& f){
			return fnmatch(name.c_str(), f.c_str(), 0) == 0;
		});
		if(res == qt_files.end()){
			return;
		}
	}
	auto path = get_path(file_info);
	if(!dir_contains(path, cmake_files)){
		return;
	}
	auto menu = caja_menu_item_new("cmake-gui", "Open project with cmake-gui", "Open project with cmake-gui", nullptr);
	caja_menu_append_item(menu_root, menu);
	g_signal_connect_object (menu, "activate",
							 G_CALLBACK (cmake_callback),
							 file_info, static_cast<GConnectFlags>(0));
}

inline bool is_qt_project_(CajaFileInfo *file_info){
	if(!caja_file_info_is_directory(file_info)){
		gchar_handle n (caja_file_info_get_name(file_info));
		std::string name =  (n == nullptr) ? "" : n.get();
		return std::find(qt_files.begin(), qt_files.end(), name) != qt_files.end();
	}

	auto path = get_path(file_info);
	return dir_contains(path, qt_files);
}

inline std::string is_qt_project(const std::vector<std::string>& files){
	for(const auto& file : qt_files){
		auto res = std::find_if(files.begin(), files.end(),
								[&file](const std::string& f){
			return fnmatch(file.c_str(), f.c_str(), 0) == 0;
		}
		);
		if(res != files.end()){
			return "\"" + get_path(*res) + "\"";
		}
	}
	return "";
}

inline std::string filter_jpg_file(const std::vector<std::string>& files){
	std::string to_return;
	for(const auto& file : files){
		if(fnmatch("*.jpg", file.c_str(), 0)==0){
			to_return += "\"" + file + "\"";
		}
	}
	return to_return;

}

inline std::string get_env(const std::string& var){
	const char* env = std::getenv(var.c_str());
	return env == nullptr ? "" : env;
}
#endif
