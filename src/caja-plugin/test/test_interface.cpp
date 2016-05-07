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

#include "catch.hpp"

// local
#include "../details.hpp"

// caja
#include <libcaja-extension/caja-extension-types.h>
#include <libcaja-extension/caja-menu-provider.h>

// glib

// cstd
#include <cstdlib>

// std
#include <iostream>

TEST_CASE("firstel", "[.]"){

	// FIXME: GLib-GObject-ERROR **: cannot create instance of abstract (non-instantiatable) type 'GTypeModule'
	auto module = reinterpret_cast<GTypeModuleClass*>(g_object_new (G_TYPE_TYPE_MODULE, nullptr));
	caja_module_initialize((GTypeModule*)module);

	int num = 0;
	const GType *types = nullptr;
	caja_module_list_types(&types, &num);
	REQUIRE(num >0);

	caja_module_shutdown();

}




