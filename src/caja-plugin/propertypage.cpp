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


// local
#include "propertypage.hpp"
#include "menu.hpp" // for cm_type
#include "utils.hpp"
#include "glib_iterator.hpp"
#include "glib_memory.hpp"

// glib
#include <glib.h>

// cstd
#include <cstring>
#include <cassert>

// std
#include <type_traits>
#include <string>
#include <fstream>
#include <vector>

// global instancens: beware, there are some other static variables
extern char **environ;

namespace propertypage{

	void list_types (const GType **types, int *num_types){
		// FIXME: validate parameters, and check that cm_type is not empty(!)
		*types = &cm_type[0];
		*num_types = static_cast<int>(cm_type.size());
	}


	void propertypage_register_type (GTypeModule *module) {

		static GTypeInfo info = {}; // global

		// if not set plugins get loaded again and again...
		info.class_size = sizeof (PropertyPageClass);
		static_assert(std::is_pod<PropertyPageClass>::value,
					  "needs to be POD since it is allocated and not inited by Caja");

		info.base_init = nullptr;
		info.base_finalize = nullptr;

		info.class_init = propertypage_class_init;
		info.class_finalize = nullptr;
		info.class_data = nullptr; // data used for initialisation

		// if not set plugins get loaded again and again...
		info.instance_size = sizeof (PropertyPage);
		static_assert(std::is_pod<PropertyPage>::value,
					  "needs to be POD since it is allocated and not inited by Caja");
		info.n_preallocs = 0;
		info.instance_init = propertypage_init;

		info.value_table = nullptr;

		static GInterfaceInfo menu_provider_iface_info = {}; // global
		menu_provider_iface_info.interface_init = propertypage_iface_init;
		menu_provider_iface_info.interface_finalize = nullptr;
		menu_provider_iface_info.interface_data = nullptr;


		GType type = g_type_module_register_type (module,
												  G_TYPE_OBJECT,
												  "ContextMenu",
												  &info, static_cast<GTypeFlags>(0));

		g_type_module_add_interface (module,
									 type,
									 CAJA_TYPE_PROPERTY_PAGE_PROVIDER, // è per questo che ricevo un CajaMenuInterface????
									 &menu_provider_iface_info);

		// alternative sono CAJA_TYPE_COLUMN_PROVIDER, CAJA_TYPE_INFO_PROVIDER, CAJA_TYPE_PROPERTY_PAGE_PROVIDER, ...
		cm_type.push_back(type);
	}

	/// Initializes PropertyPageClass
	void propertypage_class_init(gpointer g_class, gpointer class_data){
		(void) g_class; (void)class_data;
	}

	/// Initializes PropertyPage
	void propertypage_init(GTypeInstance *instance, gpointer g_class){
		(void) instance; (void)g_class;
	}

	/// register the callback function for CajaMenuProviderIface
	void propertypage_iface_init(gpointer g_iface, gpointer iface_data){
		(void)iface_data;
		// has been registered with CAJA_TYPE_MENU_PROVIDER
		auto propertypage_iface = reinterpret_cast<CajaPropertyPageProviderIface*>(g_iface);
		propertypage_iface->get_pages = get_pages;
	}


	GList* get_pages(CajaPropertyPageProvider *provider, GList *files){
		(void)provider;

		if(compare_size(files, 1) != 0){
			return nullptr;
		}

		GtkWidget* properties = gtk_label_new ("Hello World!");
		gtk_widget_show(properties);

		CajaPropertyPage* page = caja_property_page_new ("devbox::property_page",
									   gtk_label_new ("DevBox"),
									   properties);
		GList* pages = nullptr;
		pages = g_list_append (pages, page);
		return pages;
	}

}
