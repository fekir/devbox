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

/// Main function used for implementing the interface of the Caja Extension

#ifndef MY_PROPERTYPAGE_HPP
#define MY_PROPERTYPAGE_HPP

// local
#include "utils.hpp"
#include "posix_memory.hpp"

// caja
#include <libcaja-extension/caja-extension-types.h>
#include <libcaja-extension/caja-property-page-provider.h>
#include <libcaja-extension/caja-file-info.h>

// glib
#include <glib.h> // GType, GTypeModule

// cstd

// std
#include <vector>
#include <fstream>
#include <memory>
#include <functional>

// all implementation details for the caja-menu should be here
namespace propertypage{
	struct PropertyPage{
		GObject parent;
	};

	struct PropertyPageClass{
		GObjectClass parent_class;
	};

	void propertypage_register_type(GTypeModule* module);

	void list_types(const GType** types, int* num_types);

	/// Initializes PropertyPageClass
	void propertypage_class_init(gpointer g_class, gpointer class_data);

	/// Initializes PropertyPage
	void propertypage_init(GTypeInstance* instance, gpointer g_class);

	/// register the callback function for CajaMenuProviderIface
	void propertypage_iface_init(gpointer g_iface, gpointer iface_data);

	// Callback function
	GList* get_pages(CajaPropertyPageProvider* provider, GList* files);
}

#endif
