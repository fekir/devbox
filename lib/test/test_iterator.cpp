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
#include "glib_iterator.hpp"


// glib
#include <glib.h> // GList

// posix
#include <fnmatch.h> // fnmatch
#include <unistd.h>  // execve

// std
#include <iostream>

struct TestStruct{
	int el;
};

// not const even if never changed, since GList is not constsafe
TestStruct t1 = {12};
TestStruct t2 = {13};
TestStruct t3 = {14};
TestStruct t4 = {15};


GList* createlist(){
	GList* list = g_list_prepend(nullptr, &t4);
	list = g_list_prepend(list, &t3);
	list = g_list_prepend(list, &t2);
	list = g_list_prepend(list, &t1);
	return list;
}

GSList* createslist(){
	GSList* list = g_slist_prepend(nullptr, &t4);
	list = g_slist_prepend(list, &t3);
	list = g_slist_prepend(list, &t2);
	list = g_slist_prepend(list, &t1);
	return list;
}

TEST_CASE("GList_empty", "[GList][empty]") {
	// Create GList
	GList* list = createlist();
	REQUIRE(!empty(list));

	GList* list2 = nullptr;
	REQUIRE(empty(list2));
}

TEST_CASE("GSList_empty", "[GList][empty]") {
	// Create GList
	GSList* list = createslist();
	REQUIRE(!empty(list));

	GSList* list2 = nullptr;
	REQUIRE(empty(list2));
}


TEST_CASE("GList_size", "[GList][size]") {
	// Create GList
	GList* list = createlist();

	REQUIRE(size(list) == g_list_length(list));

	REQUIRE(size(list->next) == g_list_length(list));

	GList* list2 = nullptr;
	REQUIRE(size(list2) == g_list_length(list2));

	// since prev = nullptr
	REQUIRE(size(list->prev) == 0);

	_GList list3 = {};
	REQUIRE(size(&list3) == 1);
}

TEST_CASE("GList_iter1", "[GList][iterator][forward]") {

	// Create GList
	GList* list = createlist();

	// Assure that I've understood how GList works
	REQUIRE(list->prev == nullptr);
	REQUIRE(list->data == &t1);
	REQUIRE(list->next != nullptr );
	REQUIRE(list->next->data == &t2);

	// Test the iterator!
	size_t counter{};
	for(auto it = begin(list); it != end(list); ++it){
		++counter;
	}
	REQUIRE(counter == size(list));

	for(auto el : list){
		--counter;
	}
	REQUIRE(counter == 0);

}

TEST_CASE("begin", ""){
	GList* list = createlist();
	auto el = g_list_first(list->next->next);
	auto el2 = begin(list->next->next);
	REQUIRE(el->data == &t1);
	REQUIRE(el->data == *el2);
}

TEST_CASE("count2", ""){
	GList* list = createlist();
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
	GList* list = createlist();
	std::vector<TestStruct*> vec = to_vector<TestStruct>(list);
	REQUIRE(vec.size() == size(list));
	REQUIRE(vec.at(0) == list->data);
	REQUIRE(vec.at(1) == list->next->data);
}
