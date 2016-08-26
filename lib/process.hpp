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

/// This package contains utility functions for invoking programs

#ifndef H_PROCESS_0
#define H_PROCESS_0


#include "process_base.hpp"

#if defined(WIN32)

#else // FIXME: check how to detect if we are on a POSIX system, if there is no way, use CMake to configure the Project
#include "posix/process_posix.hpp"
#endif

#endif
