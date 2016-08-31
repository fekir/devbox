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


#ifndef H_FILESYSTEM_UTILS_POSIX_0
#define H_FILESYSTEM_UTILS_POSIX_0

// local
#include "string_utils.hpp"
#include "posix_memory.hpp"

// posix
#include <dirent.h> // DIR*
#include <fnmatch.h> // fnmatch

// std
#include <string>
#include <algorithm>
#include <functional>

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

inline bool file_match(const std::string& pattern, const std::string& file_name){
	int res = fnmatch(pattern.c_str(), file_name.c_str(), 0);
	assert(res == 0 || res == FNM_NOMATCH);
	return res == 0;
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
#endif
