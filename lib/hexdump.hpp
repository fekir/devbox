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

#ifndef H_HEXDUMP
#define H_HEXDUMP

#include <string>
#include <limits>       // numeric_limits
#include <cassert>
#include <stdexcept>
#include <cstring>
#include <array>
#include <algorithm>
#include <numeric>
#include <vector>
#include <ios>
#include <sstream>
#include <iomanip>
#include <cctype>
#include <iostream>

/*
	Type safe class for hexdumps
	TODO: add static_assert swap operator
	TODO: add spacing options, prepend 0x, ... (when converting to string)
	TODO: add cbegin, cend
*/

constexpr char lut[] = "0123456789ABCDEF";

namespace details{

	inline constexpr std::array<char,2> to_hex(const unsigned char c) noexcept {
		static_assert(((std::numeric_limits<unsigned char>::max() & 0xff) >> 4 )<16, "Impossible");
		static_assert(((std::numeric_limits<unsigned char>::max() & 0x0f)      )<16, "Impossible");
		return {{lut[(c & 0xff) >> 4], lut[c & 0x0f]}};
	}

	/// if random access iterator:
	///  preallocatese necessary spaces and does range check,
	/// if not, it may be too costly to traverse the whole container twice, so do not preallocate
	template<class iter>
	void preallocateifrandomaccess(const iter& begin, const iter& end, std::string& content) {
		using iter_type = typename std::iterator_traits<iter>::iterator_category;
		if(std::is_same<iter_type, std::random_access_iterator_tag>::value) {
			const auto sizetoencode = std::distance(begin, end);
			if(
			   sizetoencode < 0 ||
			   sizetoencode > std::numeric_limits<decltype(sizetoencode)>::max()/2
			   ){
				throw std::out_of_range("string is to long, unable to encode!");
			}
			content.reserve(static_cast<decltype(content.size())>(2*sizetoencode));
		}
	}

	inline bool isHexString(const std::string& content) {
		const bool isLengthValid = (content.length() % 2 == 0);
		// add check of characters
		return isLengthValid;
	}

}

class hexdump {
private:
	std::string content;
	explicit hexdump(const std::string& c) : content(c) {
		assert(details::isHexString(c));
	}

public:
	const std::string& str() const {
		return content;
	}

	friend bool operator==(const hexdump& lhs, const hexdump& rhs){
		return lhs.str() == rhs.str();
	}

	friend std::ostream& operator<<(std::ostream& os, const hexdump& d) {
		os << d.str();
		return os;
	}

	/// accepts as input a pair of iterators of type:
	///   char, unsigned char and signed char
	template<class iter>
	static hexdump dump(iter begin, iter end){
		// check iterators
		using value_type_with_qualifier = typename std::decay<decltype(*begin)>::type;
		using value_type = typename std::remove_cv<value_type_with_qualifier>::type;
		static_assert( // check type --> remove identifier
		               std::is_same<value_type,char>::value ||
		               std::is_same<value_type,signed char>::value ||
		               std::is_same<value_type,unsigned char>::value,
		               "iter has to be an iterator/pointer of char, signed char or unsigned char"
		               );

		std::string content;
		details::preallocateifrandomaccess(begin, end, content);

		for(; begin != end; ++begin){
			auto c = details::to_hex(static_cast<unsigned char>(*begin));
			content += c[0];
			content += c[1];
		}

		hexdump toreturn(content);
		return toreturn;
	}

	/// dumps content of std::string
	static hexdump dump(const std::string& todump){
		return dump(todump.begin(), todump.end());
	}
	static hexdump dump(const std::vector<unsigned char>& todump){
		return dump(todump.begin(), todump.end());
	}

	/// converts current string to hexdump, throws if current string is not a valid dump
	static hexdump to_dump(std::string d){
		std::transform(d.begin(), d.end(), d.begin(), ::toupper);
		if(!details::isHexString(d)){
			throw std::out_of_range("invalid hex");
		}
		return hexdump(d);
	}


	static std::vector<unsigned char> undump(const hexdump& dump) {
		const auto& content = dump.content;
		assert(details::isHexString(content) && "invariant failed");

		std::vector<unsigned char> output;
		output.reserve(content.length() / 2);
		for (auto i = decltype(content.length()){0}; i != content.length(); i += 2){
			assert(i < content.length());
			const char a = content.at(i);
			const char* p = std::lower_bound(std::begin(lut), std::end(lut), a);
			assert(*p == a && "not a hex digit");

			const char b = content.at(i + 1);
			const char* q = std::lower_bound(std::begin(lut), std::end(lut), b);
			assert(*q == b && "not a hex digit");

			assert((((p - lut) << 4) | (q - lut)) <= std::numeric_limits<unsigned char>::max());
			output.push_back(static_cast<unsigned char>(((p - lut) << 4) | (q - lut)));
		}
		return output;
	}


};

#endif
