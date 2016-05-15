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



/// This package provides typesafe and leaksafe "objects" for different types defined in the gtk library.
/// There are no other differences, for a correct usage, consider reading the documentation of std::unique_ptr

#ifndef H_GTK_MEMORY
#define H_GTK_MEMORY

// gtk
#include <gtk/gtk.h>

// std
#include <memory> // unique_ptr


struct GtkWidget_deleter{
	void operator()(GtkWidget* w) const {
		gtk_widget_destroy(w);
	}
};
using GtkWidget_handle = std::unique_ptr<GtkWidget, GtkWidget_deleter>;

#endif
