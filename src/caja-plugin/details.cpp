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
#include "details.hpp"
#include "glib_iterator.hpp"
#include "glib_memory.hpp"

// glib
#include <glib.h>

// cstd
#include <cstring>

// std
#include <type_traits>
#include <string>
#include <fstream>
#include <vector>

// global instancens: beware, there are some other static variables
extern char **environ;

std::vector<details::command_and_menu> cmake_parsers;

namespace details{

	std::vector<GType> cm_type; // global

	void list_types (const GType **types, int *num_types){
		// FIXME: validate parameters, and check that cm_type is not empty(!)
		*types = &cm_type[0];
		*num_types = static_cast<int>(cm_type.size());
	}


	void context_menu_register_type (GTypeModule *module) {

		static GTypeInfo info = {}; // global

		// if not set plugins get loaded again and again...
		info.class_size = sizeof (details::ContextMenuClass);
		static_assert(std::is_pod<details::ContextMenuClass>::value,
					  "needs to be POD since it is allocated and not inited by Caja");

		info.base_init = nullptr;
		info.base_finalize = nullptr;

		info.class_init = details::context_menu_class_init;
		info.class_finalize = nullptr;
		info.class_data = nullptr; // data used for initialisation

		// if not set plugins get loaded again and again...
		info.instance_size = sizeof (details::ContextMenu);
		static_assert(std::is_pod<details::ContextMenu>::value,
					  "needs to be POD since it is allocated and not inited by Caja");
		info.n_preallocs = 0;
		info.instance_init = details::context_menu_init;

		info.value_table = nullptr;

		static GInterfaceInfo menu_provider_iface_info = {}; // global
		menu_provider_iface_info.interface_init = details::menu_provider_iface_init;
		menu_provider_iface_info.interface_finalize = nullptr;
		menu_provider_iface_info.interface_data = nullptr;


		GType type = g_type_module_register_type (module,
												  G_TYPE_OBJECT,
												  "ContextMenu",
												  &info, static_cast<GTypeFlags>(0));

		g_type_module_add_interface (module,
									 type,
									 CAJA_TYPE_MENU_PROVIDER, // è per questo che ricevo un CajaMenuInterface????
									 &menu_provider_iface_info);

		// alternative sono CAJA_TYPE_COLUMN_PROVIDER, CAJA_TYPE_INFO_PROVIDER, ...
		cm_type.push_back(type);


		// add cmake parser, there should be a check for getting list of installed programs...
		cmake_parsers.emplace_back("/usr/bin/cmake-gui", is_cmake_project_);
		cmake_parsers.emplace_back("/usr/bin/qtcreator", is_cmake_project_);

	}

	void context_menu_class_init(gpointer g_class, gpointer class_data){
		(void) g_class; (void)class_data;
	}

	void context_menu_init(GTypeInstance *instance, gpointer g_class){
		(void) instance; (void)g_class;
	}

	void menu_provider_iface_init (gpointer g_iface, gpointer iface_data){
		(void)iface_data;
		auto menu_provider_iface = reinterpret_cast<CajaMenuProviderIface*>(g_iface);
		menu_provider_iface->get_file_items = details::get_file_items;
		menu_provider_iface->get_background_items = details::get_background_items;
	}

	GList* get_file_items(CajaMenuProvider *provider, GtkWidget *window, GList *files) {
		(void)provider;
		(void)window;
		// Notice: if files==nullptr, it does not mean that someone clicked on the background
		// (see get_background_items function for that use case)
		if(compare_size(files, 1)!=0){
			return nullptr;
		}

		auto file = reinterpret_cast<CajaFileInfo*>(files->data);
		if(file==nullptr){
			return nullptr;
		}

		GList* to_return = nullptr;

		CajaMenuItem* menu_item_root = caja_menu_item_new("CajaDevelopment::Development", "Development", "Development submenu", "");
		auto menu_root = caja_menu_new();
		caja_menu_item_set_submenu(menu_item_root, menu_root);

		//commands_to_execute.clear();
		if(create_menu_items(*menu_root, *file, cmake_parsers)==0){
			return nullptr;
		}
		to_return = g_list_append (to_return, menu_item_root);
		return to_return;
	}

	GList* get_background_items(CajaMenuProvider *provider, GtkWidget *window, CajaFileInfo *current_folder){
		(void)provider;(void)window;
		if(current_folder== nullptr){
			return nullptr;
		}

		GList* to_return = nullptr;

		CajaMenuItem* menu_item_root = caja_menu_item_new("CajaDevelopment::Development", "Development", "Development submenu", "");
		auto menu_root = caja_menu_new();
		caja_menu_item_set_submenu(menu_item_root, menu_root);

		if(create_menu_items(*menu_root, *current_folder, cmake_parsers)==0){
			return nullptr;
		}
		to_return = g_list_append (to_return, menu_item_root);
		return to_return;
	}

	void menu_callback (CajaMenuItem *item, CajaFileInfo *file) {
		(void)item;
		(void)file;


		GtkWidget *dialog = gtk_message_dialog_new_with_markup (
					nullptr, static_cast<GtkDialogFlags>(0),
					GTK_MESSAGE_INFO, GTK_BUTTONS_CLOSE,
					"<big><b>You have selected this element.</b></big>\n\n"
					"Too bad it doesn't do anything useful...");
		gtk_dialog_run (GTK_DIALOG(dialog));
		gtk_widget_destroy (dialog); // FIXME: put in destructor
	}

	void menu_callback_cmake (CajaMenuItem *item, CajaFileInfo *file){
		const auto prog = reinterpret_cast<const char*>(g_object_get_data(G_OBJECT (item), "fekir::command"));
		assert(prog != nullptr);
		std::string program = prog;
		std::string param = get_path(file);
		char* const args[] = {&program[0], &param[0],  nullptr};
		//char* const env[] = {&display[0], nullptr};
		const pid_t child_pid = fork();
		if (child_pid == 0) {  // in child
			execve(args[0], args, environ); // check != -1
		}
	}

	void generic_gui_callback(CajaMenuItem* item, gpointer file_){
		(void)item;
		const auto ptr = reinterpret_cast<command_to_execute*>(file_);
		command_to_execute comtoex = *ptr;

		std::string program = comtoex.program;
		auto argv = to_argv(comtoex.arguments);
		argv.insert(argv.begin(), &program[0]);
		const pid_t child_pid = fork();
		if (child_pid == 0) {  // in child
			execve(&program[0], &argv[0], environ); // check != -1
		}
	}

	// apparently neve called, doens't caja reset the connection to a signal?
	void closureNotify(gpointer data_, GClosure *closure){
		(void)closure;
		auto data =reinterpret_cast<command_to_execute*>(data_);
		delete data; // allocated with new
	}

	std::size_t create_menu_items(CajaMenu& menu_root, CajaFileInfo& file_info, std::vector<command_and_menu>& parsers){
		std::size_t to_return = 0;

		for(auto& v : parsers){
			bool add_el = (v.check_if_add == nullptr) ? true : v.check_if_add(&file_info);
			if(!add_el){
				continue;
			}
			const std::string program = !v.menu_title.empty() ? v.menu_title :
																get_name(v.cmake_command); // fixme, only command, no path
			const std::string title = "fekir::" + program;
			const std::string label = !v.menu_label.empty() ? v.menu_label :
															  "Open file with " + program;
			const std::string tip = !v.menu_tip.empty() ? v.menu_tip :
														  "Open file with " + program;\
			CajaMenuItem* menu = caja_menu_item_new
					(title.c_str(), label.c_str(), tip.c_str(), v.menu_icon.c_str());

			// these info needs to be generated from command_and_menu -- currently works for cmake and qtcreator
			command_to_execute* toadd2 = new command_to_execute;
			toadd2->arguments.push_back(get_path(&file_info));
			toadd2->program = v.cmake_command;

			g_signal_connect_data(menu, "activate",
								  G_CALLBACK (details::generic_gui_callback),
								  static_cast<gpointer>(toadd2), &closureNotify, static_cast<GConnectFlags>(0));
			// static_assert that file is the same type of 2nd param of callback function!
			caja_menu_append_item(&menu_root, menu);
			++to_return;
		}
		return to_return;
	}

}
