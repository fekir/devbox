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

/// This package provides an interface for the GList and GSList containers to the <iterator> library


#ifndef H_GLIB_ITERATOR
#define H_GLIB_ITERATOR

// glib
#include <glib.h> // GList, GSList

// cstd
#include <cstddef> // size_t
#include <cassert>

// std
#include <utility> // swap
#include <iterator> // back_insert_iterator
#include <vector>
#include <algorithm>


#define is_glist_or_sglist(glist) static_assert(std::is_same<glist,GList>::value || std::is_same<glist,GSList>::value, "function overload only for GList and GSList")

/// Return true if a GList/GSList is empty, false otherwise
/// Overload of the std::empty function
template<class glist>
inline bool empty(const glist* list){
	is_glist_or_sglist(glist);
	return list == nullptr;
}

/// Returns the number of elements of a GList
/// Unlike g_list_length, this function is const-correct, and it returns always the same size, even if not pointing to the first element
/// Overload of the std::size function
inline std::size_t size(const GList* list){
	if(list == nullptr){
		return 0;
	}
	std::size_t counter = 1;
	auto el = list->next;
	while(el != nullptr){
		++counter;
		el = el->next;
	}
	el = list->prev;
	while(el != nullptr){
		++counter;
		el = el->prev;
	}
	return counter;
}

/// Returns the number of elements of a GList
/// Unlike g_list_length, this function is const-correct
/// Unlike the size-function for GList, the size is "incorrect" if the input parameter does not point to the first element
/// Overload of the std::size function
inline std::size_t size(const GSList* list){
	if(list == nullptr){
		return 0;
	}
	std::size_t counter = 1;
	auto el = list->next;
	while(el != nullptr){
		++counter;
		el = el->next;
	}
	return counter;
}

/// Since calculating the lenght of a GList is a O(n) operation, where n is the size of the GList.
/// you should use this function if you want to check the number of elements against a specific number
/// This function returns > 0 if the list has more elements than asked and <0 if the number of elements is less, 0 if equals.
/// The complexity is O(min(to_compare, n))
inline int compare_size(const GList* list, std::size_t to_compare){
	if(list == nullptr){
		return to_compare == 0 ? 0 : -1; // to_compare >= size
	}
	std::size_t counter = 1;
	auto el = list->next;
	while(el != nullptr){
		if(counter > to_compare){
			return 1;
		}
		++counter;
		el = el->next;
	}
	el = list->prev;
	while(el != nullptr){
		if(counter > to_compare){
			return 1;
		}
		++counter;
		el = el->prev;
	}
	return
	        counter > to_compare ? 1 :
	        counter < to_compare ? -1 :
	        0;
}

/// Since calculating the lenght of a GSList is a O(n) operation, where n is the size of the GSList,
/// you should use this function if you want to check the number of elements against a specific number
/// This function returns > 0 if the list has more elements than asked and <0 if the number of elements is less, 0 if equals.
/// The complexity is O(min(to_compare, n))
inline int compare_size(const GSList* list, std::size_t to_compare){
	if(list == nullptr){
		return to_compare == 0 ? 0 : -1; // to_compare >= size
	}
	std::size_t counter = 1;
	auto el = list->next;
	while(el != nullptr){
		if(counter > to_compare){
			return 1;
		}
		++counter;
		el = el->next;
	}
	return
	        counter > to_compare ? 1 :
	        counter < to_compare ? -1 :
	        0;
}



/// This class provides a ForwardIterator interface for GList/GSList
template<class glist>
class GListForwardIterator {
	const glist* node = nullptr;
public:
	explicit GListForwardIterator() {}
	explicit GListForwardIterator(const glist* p) : node(p) {
		is_glist_or_sglist(glist);
	}
	GListForwardIterator(const GListForwardIterator& other) : node(other.node) {}
	GListForwardIterator& operator=(GListForwardIterator other) { std::swap(node, other.node); return *this; }

	bool operator==(const GListForwardIterator& other) const { return node == other.node; }
	bool operator!=(const GListForwardIterator& other) const { return node != other.node; }

	const gpointer& operator*() const { assert(node!=nullptr); return node->data; } // FIXME: add type deduction for return value

	GListForwardIterator& operator++() { assert(node!=nullptr); node = node->next; return *this;}
	GListForwardIterator operator++(int) { assert(node!=nullptr); GListForwardIterator it(*this);node = node->next; return it;}

	// actually this is not required by the Forward iterator interface
	GListForwardIterator operator+(int i) {
		GListForwardIterator iter = *this;
		while (i-- > 0) {
			++iter;
		}
		return iter;
	}
};

/// Returns a ForwardIterator to the beginning of the GList
/// If the input paramter does not already point to the first element, the function will iterate backwards
/// Overload of the std::begin function
inline GListForwardIterator<GList> begin(GList* list){
	while(list->prev != nullptr){
		list = list->prev;
	}
	return GListForwardIterator<GList>(list);
}
inline GListForwardIterator<GList> begin(const GList* list){
	while(list->prev != nullptr){
		list = list->prev;
	}
	return GListForwardIterator<GList>(list);
}

/// Returns a ForwardIterator to the beginning* of the GSList
/// (*)Notice that if the input paramter does not point to the first element, the current element will be treated as first
/// Overload of the std::begin function
inline GListForwardIterator<GSList> begin(GSList* list){
	return GListForwardIterator<GSList>(list);
}
inline GListForwardIterator<GSList> begin(const GSList* list){
	return GListForwardIterator<GSList>(list);
}

/// Returns an iterator to the end (i.e. the element after the last element) of of a GList/GSList
/// Overload of the std::end function
template<class glist>
inline GListForwardIterator<glist> end(glist* list){
	is_glist_or_sglist(glist);
	(void)list;
	return GListForwardIterator<glist>(nullptr);
}
template<class glist>
inline GListForwardIterator<glist> end(const glist* list){
	is_glist_or_sglist(glist);
	(void)list;
	return GListForwardIterator<glist>(nullptr);
}

/// Converts a std::vector<T*> to a GList.
/// T need to be a POD type (maybe this restriction is not necessary)
template<class T>
GList* to_GList(const std::vector<T*>& vec){
	static_assert(std::is_pod<T>::value , "needs to be POD");
	GList* toreturn = nullptr;
	for(const auto& v : vec){
		toreturn = g_list_append(toreturn, v);
	}
	return toreturn;
}

/// Converst a GList/GSList to a ype safe std::vector
/// It performs a unsafe cast, unless you are asking a std::vector<void*>
/// T needs to be a POD type
template<class T, class glist>
std::vector<T*> to_vector(const glist* list){
	is_glist_or_sglist(glist);
	std::vector<T*> to_return;
	std::transform(begin(list), end(list), std::back_insert_iterator<std::vector<T*>>(to_return),
	               [](const gpointer p){return reinterpret_cast<T*>(p);}
	);
	return to_return;
}

#endif
