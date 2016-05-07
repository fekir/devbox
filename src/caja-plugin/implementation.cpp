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

/// This file contains the implementation of the caja-menu-provider interface

// local
#include "details.hpp"

// caja
#include <libcaja-extension/caja-extension-types.h>
#include <libcaja-extension/caja-menu-provider.h>

// std
#include <fstream>

// --- extension interface ---
void caja_module_initialize (GTypeModule *module){
	details::context_menu_register_type(module);
}

void caja_module_shutdown (){
}

void caja_module_list_types (const GType **types, int *num_types){
	details::list_types(types, num_types);
}
