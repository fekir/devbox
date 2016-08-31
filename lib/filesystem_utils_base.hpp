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


#ifndef H_FILESYSTEM_UTILS_BASE_0
#define H_FILESYSTEM_UTILS_BASE_0

// local
#include "string_utils.hpp"

// cstd
#include <cassert>

// std
#include <string>
#include <algorithm>


#if defined(WIN32)
const char     path_delimiters[] =  "/\\";
const wchar_t wpath_delimiters[] = L"/\\";
#else
const char     path_delimiters[] =  "/";
const wchar_t wpath_delimiters[] = L"/";
#endif

/// return current path if a directory, path to file if file
/// a path is a directory if it ends with the path delimiter
inline std::string get_path(const std::string& file_info){
	const size_t last_slash = file_info.find_last_of(path_delimiters);
	if(std::string::npos == last_slash) {
		return "";
	}
	return file_info.substr(0, last_slash + 1);
}

inline std::wstring get_path(const std::wstring& file_info){
	const size_t last_slash = file_info.find_last_of(wpath_delimiters);
	if(std::string::npos == last_slash) {
		return L"";
	}
	return file_info.substr(0, last_slash + 1);
}

/// return name of file
/// removes path if present
inline std::string get_name(const std::string& file_info){
	const size_t last_slash = file_info.find_last_of(path_delimiters);
	if(std::string::npos == last_slash) {
		return file_info;
	}
	return file_info.substr(last_slash+1);
}

inline std::wstring get_name(const std::wstring& file_info){
	const size_t last_slash = file_info.find_last_of(wpath_delimiters);
	if(std::string::npos == last_slash) {
		return file_info;
	}
	return file_info.substr(last_slash+1);
}

#endif
