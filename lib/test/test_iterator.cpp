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
#include "glib_iterator.hpp"
#include "glib_memory.hpp"


// glib
#include <glib.h> // GList

// std
#include <iostream>

namespace test{

	struct TestStruct{
		int el;
	};

	// not const even if never changed, since GList is not constsafe
	TestStruct t1 = {12};
	TestStruct t2 = {13};
	TestStruct t3 = {14};
	TestStruct t4 = {15};


	GList_handle create_glist(){
		GList* list = g_list_prepend(nullptr, &t4);
		list = g_list_prepend(list, &t3);
		list = g_list_prepend(list, &t2);
		list = g_list_prepend(list, &t1);
		GList_handle handle(list);
		return handle;
	}

	GSList_handle create_sglist(){
		GSList* list = g_slist_prepend(nullptr, &t4);
		list = g_slist_prepend(list, &t3);
		list = g_slist_prepend(list, &t2);
		list = g_slist_prepend(list, &t1);
		GSList_handle handle(list);
		return handle;
	}

	TEST_CASE("GList_empty", "[GList][empty]") {
		// Create GList
		const auto list = create_glist();
		REQUIRE(!empty(list.get()));

		GList* list2 = nullptr;
		REQUIRE(empty(list2));
	}

	TEST_CASE("GSList_empty", "[GList][empty]") {
		// Create GList
		const auto list = create_sglist();
		REQUIRE(!empty(list.get()));

		GSList* list2 = nullptr;
		REQUIRE(empty(list2));
	}


	TEST_CASE("GList_size", "[GList][size]") {
		// Create GList
		auto list_ = create_glist();
		auto list = list_.get();

		REQUIRE(size(list) == g_list_length(list));

		REQUIRE(size(list->next) == g_list_length(list));

		GList* list2 = nullptr;
		REQUIRE(size(list2) == g_list_length(list2));

		// since prev = nullptr
		REQUIRE(size(list->prev) == 0);

		GList list3 = {};
		REQUIRE(size(&list3) == 1);
	}

	TEST_CASE("GList_iter1", "[GList][iterator][forward]") {

		// Create GList
		auto list = create_glist();

		// Assure that I've understood how GList works
		REQUIRE(list->prev == nullptr);
		REQUIRE(list->data == &t1);
		REQUIRE(list->next != nullptr);
		REQUIRE(list->next->data == &t2);

		// Test the iterator!
		size_t counter{};
		for(auto it = begin(list.get()); it != end(list.get()); ++it){
			++counter;
		}
		REQUIRE(counter == size(list.get()));

		for(auto el : list.get()){
			--counter;
		}
		REQUIRE(counter == 0);

	}

	TEST_CASE("begin", ""){
		auto list = create_glist();
		auto el = g_list_first(list->next->next);
		auto el2 = begin(list->next->next);
		REQUIRE(el->data == &t1);
		REQUIRE(el->data == *el2);
	}

	TEST_CASE("count2", ""){
		const auto list_ = create_glist();
		auto list = list_.get();
		REQUIRE(compare_size(list, 2) > 0);
		REQUIRE(compare_size(list, 5) < 0);
		REQUIRE(compare_size(list, 4) == 0);
	}

	TEST_CASE("count3", ""){
		GList* list = nullptr;
		REQUIRE(compare_size(list, 5) < 0);
		REQUIRE(compare_size(list, 0) == 0);
	}

	TEST_CASE("to_vector", ""){
		const auto list = create_glist();
		const std::vector<TestStruct*> vec = to_vector<TestStruct>(list.get());
		REQUIRE(vec.size() == size(list.get()));
		REQUIRE(vec.at(0) == list->data);
		REQUIRE(vec.at(1) == list->next->data);
	}

}
