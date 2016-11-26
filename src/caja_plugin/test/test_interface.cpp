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

#include "catch.hpp"

// local
#include "../menu.hpp"

// caja
#include <libcaja-extension/caja-extension-types.h>
#include <libcaja-extension/caja-menu-provider.h>

// glib

// cstd
#include <cstdlib>

// std
#include <iostream>


// http://www.gonwan.com/2011/03/12/oop-using-gobject-9-a-dynamic-type/

// module object struct
typedef struct _FakeModule {
	GTypeModule parent;
} FakeModule;

// module class struct
typedef struct _FakeModuleClass {
	GTypeModuleClass parent;
} FakeModuleClass;


gboolean fake_module_load(GTypeModule*) {
	return TRUE;
}

void fake_module_unload(GTypeModule*) {
}

static void fake_module_class_init(gpointer klass, gpointer) {
	GTypeModuleClass* module_class = G_TYPE_MODULE_CLASS(klass);
	module_class->load = fake_module_load;
	module_class->unload = fake_module_unload;
}

static void fake_module_instance_init(GTypeInstance*, gpointer) {
}

GType fake_module_get_type() {
	static GType type_id = 0;
	if (type_id == 0) {
		static const GTypeInfo type_info = {
		    sizeof(FakeModuleClass),   // class_size
		    nullptr,                   // base_init
		    nullptr,                   // base_finalize
		    fake_module_class_init,    // class_init
		    nullptr,                   // class_finalize
		    nullptr,                   // class_data
		    sizeof(FakeModule),        // instance_size
		    0,                         // n_preallocs
		    fake_module_instance_init, // instance_init
		    nullptr                    // value_table
		};
		type_id = g_type_register_static(
		    G_TYPE_TYPE_MODULE, "FakeModuleStaticClass", &type_info, static_cast<GTypeFlags>(0));
	}
	return type_id;
}

namespace test{

	TEST_CASE("firstel", ""){

		// FIXME: GLib-GObject-ERROR **: cannot create instance of abstract (non-instantiatable) type 'GTypeModule'
		auto module = reinterpret_cast<GTypeModule*>(g_object_new(fake_module_get_type(), nullptr));
		//fake_module_class_init(module, nullptr);
		caja_module_initialize(module);
		int num = 0;
		const GType* types = nullptr;
		caja_module_list_types(&types, &num);
		REQUIRE(num > 0);

		caja_module_shutdown();

	}
}
