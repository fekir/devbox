//
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

/// This package constains filesystem utility functions
/// Notice that most function do not validate filenames, but simply operate on the string


#ifndef H_FILESYSTEM_UTILS_0
#define H_FILESYSTEM_UTILS_0

#include "filesystem_utils_base.hpp"

#if defined(WIN32)
//#include "windows/filesystem_windows.hpp"
#else // FIXME: check how to detect if we are on a POSIX system, if there is no way, use CMake to configure the Project
#include "posix/filesystem_utils_caja.hpp"
#include "posix/filesystem_utils_posix.hpp"
#endif

#endif
