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

#ifndef MY_MENU_HPP
#define MY_MENU_HPP

// local
#include "utils.hpp"
#include "posix_memory.hpp"

// caja
#include <libcaja-extension/caja-extension-types.h>
#include <libcaja-extension/caja-menu-provider.h>

// glib
#include <glib.h> // GType, GTypeModule

// cstd

// std
#include <vector>
#include <fstream>
#include <memory>
#include <functional>

static std::vector<GType> cm_type; // global

// all implementation details for the caja-menu should be here
namespace menu{
	struct ContextMenu{
		GObject parent;
	};

	struct ContextMenuClass{
		GObjectClass parent_class;
	};

	void context_menu_register_type(GTypeModule* module);

	void list_types(const GType** types, int* num_types);

	/// Initializes ContextMenuClass
	void context_menu_class_init(gpointer g_class, gpointer class_data);

	/// Initializes ContextMenu
	void context_menu_init(GTypeInstance* instance, gpointer g_class);

	/// register the callback function for CajaMenuProviderIface
	void menu_provider_iface_init(gpointer g_iface, gpointer iface_data);


	/// called many times
	/// receives the list of selected files/background
	/// creates the list of menu element and callback functions when element is clicked
	GList* get_file_items(CajaMenuProvider* provider, GtkWidget* window, GList* files);

	GList* get_background_items(CajaMenuProvider* provider, GtkWidget* window, CajaFileInfo* current_folder);



	/// Structure currently used for storing information about the menu item, and information throug the program to launch
	/// The callback function is used to determine if given a certain file, the menu entry should appear,
	/// For example the option with cmake-gui should appear only if a cmake file is present.
	using check_id_add_func = std::function<std::vector<std::string>(const std::vector<CajaFileInfo*>&)>;
	struct command_and_menu{
		std::string program;       // program name, inclusive of path
		std::string menu_title;    // title of menu, if empty will take program name, exclusive of path
		std::string menu_label;    // label of menu, if empty will take title
		std::string menu_tip;      // label of menu, if empty will take "open with $menu_title"
		std::string menu_icon;     // icon of menu, if empty none is shown
		check_id_add_func check_if_add;
		bool executeinterminal = true;
		explicit command_and_menu(const std::string& program_, const check_id_add_func& func_, const bool executeinterminal_) :
		    program(program_), check_if_add(func_), executeinterminal(executeinterminal_) {}
	};

	struct command_to_execute{
		std::string program;
		std::vector<std::string> arguments;
	};

	inline std::vector<char*> to_argv(std::vector<std::string>& arguments) {
		std::vector<char*> toreturn;
		for(auto& arg : arguments){
			toreturn.push_back(&arg[0]);
		}
		toreturn.push_back(nullptr);
		return toreturn;
	}

	std::size_t create_menu_items(CajaMenu& menu_root, const std::vector<CajaFileInfo*>& file_infos,
	                              const std::vector<command_and_menu>& parsers);


	/// Registered callback function when user clicks on menu item
	void menu_callback(CajaMenuItem* item, CajaFileInfo* file);
	void generic_gui_callback(CajaMenuItem* item, gpointer file_);
	void generic_mateterm_callback(CajaMenuItem* item, gpointer ptr);
}

#endif
