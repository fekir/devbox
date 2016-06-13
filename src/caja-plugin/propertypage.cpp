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

// local
#include "propertypage.hpp"
#include "menu.hpp" // for cm_type
#include "utils.hpp"
#include "string_utils.hpp"
#include "glib_iterator.hpp"
#include "glib_memory.hpp"
#include "gtk_memory.hpp"
#include "openssl_memory.hpp"
#include "hexdump.hpp"
#include "process.hpp"

// posix
#include <sys/stat.h>

// openssl
#include <openssl/evp.h>

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
#include <iostream>
#include <iterator>
#include <utility>

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

	/// returns a widget where GTK_IS_TEXT_VIEW is true
	/// the widget is not editable and wrap_mode is set on WRAP_CHAR
	GtkWidget_handle create_ro_wrap_text_view(){
		GtkWidget_handle text_view(gtk_text_view_new());
		assert(GTK_IS_TEXT_VIEW(text_view.get()));
		gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view.get()), FALSE);
		gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view.get()), GTK_WRAP_CHAR);
		return text_view;
	}

	GtkWidget_handle create_hash_sum(CajaFileInfo* file_info){
		if(caja_file_info_is_directory(file_info)){
			return nullptr;
		}


		const std::string path_file = get_path(file_info) + "/" + get_name(file_info);

		GtkWidget_handle box_(gtk_vbox_new(FALSE, 5));
		auto box = box_.get();
		assert(GTK_IS_CONTAINER(box)); // FIXME: is it possible to static_assert??

		// TODO: if file is big, I should maybe create a task that runs in background and updates the values when ready...

		std::ifstream input( path_file, std::ios::binary );
		if(!input){
			gtk_container_add(GTK_CONTAINER(box), gtk_label_new ("Unable to open file for creating hash"));
			return box_;
		}

		std::vector<EVP_MD_CTX_HANDLE> evps; // init-list does not work...
		evps.emplace_back(make_EVP_MD_CTX_HANDLE(EVP_md5()));
		evps.emplace_back(make_EVP_MD_CTX_HANDLE(EVP_sha1()));
		evps.emplace_back(make_EVP_MD_CTX_HANDLE(EVP_sha256()));
		evps.emplace_back(make_EVP_MD_CTX_HANDLE(EVP_sha512()));

		std::array<unsigned char, 2048> buffer;
		while(!input.eof()){
			const auto readed = input.read(reinterpret_cast<char*>(&buffer.at(0)), buffer.size()).gcount();

			if(input.fail() && !input.eof()){ // something went wrong, but we where not at the end of the file
				gtk_container_add(GTK_CONTAINER(box), gtk_label_new ("An error happende while reading the file"));
				return box_;
			}

			for(auto it = evps.begin(); it != evps.end();  ){
				const auto res_update = EVP_DigestUpdate(it->get(), &buffer.at(0), readed);
				if(res_update != 1){
					const std::string hashtype = to_string(EVP_MD_CTX_md(it->get()));
					std::string error_msg = "Unable to create hash of type " + hashtype + ".";
					gtk_container_add(GTK_CONTAINER(box), gtk_label_new (error_msg.c_str()));
					it = evps.erase(it);
				} else {
					++it;
				}
			}
		}

		// finished processing file -> call finalize and get the value

		for(auto it = evps.begin(); it != evps.end();  ){
			const std::string hashtype = to_string(EVP_MD_CTX_md(it->get()));

			const int size_ = EVP_MD_CTX_size(it->get());
			std::vector<unsigned char> md_value(std::max(size_, 0));
			unsigned int md_len = static_cast<unsigned int>(md_value.size());
			const auto res_final = EVP_DigestFinal_ex(it->get(), &md_value.at(0), &md_len);
			if(res_final != 1){
				std::string error_msg = "Unable to create hash of type " + hashtype + ".";
				gtk_container_add(GTK_CONTAINER(box), gtk_label_new (error_msg.c_str()));
				it = evps.erase(it);
				continue;
			}
			md_value.resize(md_len);
			const auto dump = hexdump::dump(md_value.begin(), md_value.end());

			// add dump and description to container
			std::string description = hashtype + ":";
			gtk_container_add(GTK_CONTAINER(box), gtk_label_new(description.c_str()));
			auto text_view = create_ro_wrap_text_view();
			auto buffer_ = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text_view.get()));
			gtk_text_buffer_set_text(buffer_, dump.str().c_str(), static_cast<int>(dump.str().size()));
			gtk_container_add(GTK_CONTAINER(box), text_view.release());
			++it;
		}
		return box_;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	GList* get_pages(CajaPropertyPageProvider *provider, GList *files){
		(void)provider;

		if(compare_size(files, 1) != 0){
			return nullptr;
		}

		const auto file_info = reinterpret_cast<CajaFileInfo*>(files->data);
		const auto mimetype = get_mimetype(file_info);

		GList* pages = nullptr;

		if(mimetype == mimetype_sharedlib || mimetype == mimetype_exec){
			exec_params p;
			p.args = {get_path(file_info) + "/" + get_name(file_info)};
			p.captureoutput = true;

			const auto res = fork_and_execute("/usr/bin/hardening-check", p);
			if(res.status!=0){
				return nullptr;
			}

			std::string result = trim(res.output);

			GtkWidget_handle box_(gtk_vbox_new(FALSE, 5));
			auto box = box_.get();
			assert(GTK_IS_CONTAINER(box)); // FIXME: is it possible to static_assert??
			auto text_view = create_ro_wrap_text_view();
			auto buffer_ = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text_view.get()));
			gtk_text_buffer_set_text(buffer_, result.c_str(), static_cast<int>(result.size()));
			gtk_container_add(GTK_CONTAINER(box), gtk_label_new("Output of hardening-check:"));
			gtk_container_add(GTK_CONTAINER(box), text_view.release());

			gtk_widget_show_all(box);
			CajaPropertyPage* page = caja_property_page_new ("devbox::property_page",
			                                                 gtk_label_new ("Info"),
			                                                 box_.release());
			pages = g_list_append (pages, page);
		}
		auto hashes = create_hash_sum(file_info);
		if(hashes!=nullptr){
			gtk_widget_show_all(hashes.get());
			CajaPropertyPage* page = caja_property_page_new ("devbox::property_page",
			                                                 gtk_label_new ("Checksum"),
			                                                 hashes.release());
			pages = g_list_append (pages, page);
		}
		return pages;
	}

}
