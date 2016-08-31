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

/// This package constains filesystem utility functions for the caja module


#ifndef H_FILESYSTEM_UTILS_CAJA_0
#define H_FILESYSTEM_UTILS_CAJA_0

// local
#include "glib_memory.hpp"
#include "string_utils.hpp"

// caja
#include <libcaja-extension/caja-file-info.h>
#include <libcaja-extension/caja-menu.h>

// std
#include <string>

const std::string caja_desktop("x-caja-desktop:///");

/// return current path if a directory, path to file if file, also handles x-caja-desktop
/// on failure (smb:///, sftp, ...) returns empty string
inline std::string get_path(CajaFileInfo* file_info){
	if(caja_file_info_is_directory(file_info)){
		const gchar_handle uri(caja_file_info_get_uri(file_info));
		const std::size_t len = std::strlen(uri.get());
		if(len >= caja_desktop.size() && caja_desktop.compare(0, caja_desktop.size(), uri.get(), caja_desktop.size()) == 0){
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


inline std::string get_name(CajaFileInfo* file_info){
	gchar_handle n(caja_file_info_get_name(file_info));
	return  to_string(n.get());
}
#endif
