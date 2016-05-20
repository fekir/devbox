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


/// This package provides typesafe and leaksafe "objects" for different types defined in the posix library.
/// There are no other differences, for a correct usage, consider reading the documentation of std::unique_ptr

#ifndef H_POSIX_MEMORY
#define H_POSIX_MEMORY

// posix
#include <dirent.h> // DIR*

// cstd
#include <cstdio>

// std
#include <memory> // unique_ptr

struct DIR_deleter{
	void operator()(DIR* handle) const {
		const auto res = closedir(handle); (void)res;
	}
};
using DIR_handle = std::unique_ptr<DIR, DIR_deleter>;


struct FILE_deleter{
	void operator()(FILE* handle) const {
		const auto res = fclose(handle); (void)res;
	}
};
/// handle that closes automatically a file with fclose
using FILE_handle = std::unique_ptr<FILE, FILE_deleter>;


struct POPEN_deleter{
	void operator()(FILE* handle) const {
		const auto res = pclose(handle); (void)res;
	}
};
// handle that closes automatically a file with pclose, use for handles created with popen
using POPEN_handle = std::unique_ptr<FILE, POPEN_deleter>;

#endif
