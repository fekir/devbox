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

/// This package provides typesafe and leaksafe "objects" for different types defined in the glib library.
/// There are no other differences, for a correct usage, consider reading the documentation of std::unique_ptr

#ifndef H_GLIB_MEMORY
#define H_GLIB_MEMORY

// glib
#include <glib.h> // GList and other stuff

// std
#include <memory> // unique_ptr


struct gpointer_deleter{
	void operator()(const gpointer handle) const {
		g_free(handle);
	}
};
using gpointer_handle = std::unique_ptr<gpointer, gpointer_deleter>;
using gchar_handle    = std::unique_ptr<gchar,    gpointer_deleter>;


struct GList_deleter{
	void operator()(GList* gl) const {
		g_list_free(gl);
	}
};
using GList_handle = std::unique_ptr<GList, GList_deleter>;

struct GSList_deleter{
	void operator()(GSList* gl) const {
		g_slist_free(gl);
	}
};
using GSList_handle = std::unique_ptr<GSList, GSList_deleter>;

#endif
