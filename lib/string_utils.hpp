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

/// This package utility functions for the std::string class


#ifndef H_STRING_UTILS_0
#define H_STRING_UTILS_0

// cstd
#include <cstring>
#include <cassert>

// std
#include <string>
#include <algorithm>
#include <vector>


// array with common spaces, used by trim functions. A lot of UTF characters are missing.
constexpr char char_to_trim[] = { ' ', '\0', '\t', '\n', '\r', '\v', '\f'};
constexpr wchar_t wchar_to_trim[] = { L' ', L'\0', L'\t', L'\n', L'\r', L'\v', L'\f'};


template<class T, size_t N>
constexpr size_t array_size(const T(&)[N]) noexcept {return N;}

static_assert(array_size(char_to_trim) == array_size(wchar_to_trim), "needs to have same elements, but size is different");


inline std::string rtrim(const std::string& s){
	const auto last = s.find_last_not_of(char_to_trim, std::string::npos, array_size(char_to_trim));
	return last == std::string::npos ? "" : s.substr(0, last+1);
}
inline std::wstring rtrim(const std::wstring& s) {
	const auto last = s.find_last_not_of(wchar_to_trim, std::wstring::npos, array_size(wchar_to_trim));
	return last == std::wstring::npos ? L"" : s.substr(0, last + 1);
}

inline std::string ltrim(const std::string& s){
	const auto first = s.find_first_not_of(char_to_trim, 0, array_size(char_to_trim));
	return first == std::string::npos ? "" : s.substr(first);
}
inline std::wstring ltrim(const std::wstring& s){
	const auto first = s.find_first_not_of(wchar_to_trim, 0, array_size(wchar_to_trim));
	return first == std::wstring::npos ? L"" : s.substr(first);
}

inline std::string trim(const std::string& s){
	// not calling ltrim and rtrim for avoiding one reallocation -> it may be a premature optimisation
	const auto first = s.find_first_not_of(char_to_trim, 0, array_size(char_to_trim));
	if(first == std::string::npos){
		return "";
	}
	const auto last = s.find_last_not_of(char_to_trim, std::string::npos, array_size(char_to_trim));
	assert(last >= first); assert(last != std::string::npos);
	return s.substr(first, last-first+1);
}

inline std::wstring trim(const std::wstring& s) {
	// not calling ltrim and rtrim for avoiding one reallocation -> it may be a premature optimisation
	const auto first = s.find_first_not_of(wchar_to_trim, 0, array_size(wchar_to_trim));
	if (first == std::wstring::npos) {
		return L"";
	}
	const auto last = s.find_last_not_of(wchar_to_trim, std::wstring::npos, array_size(wchar_to_trim));
	assert(last >= first); assert(last != std::wstring::npos);
	return s.substr(first, last - first + 1);
}



inline bool start_with(const std::string& big, const std::string& smalls) {
	return smalls.length() <= big.length()
		&& std::equal(smalls.begin(), smalls.end(), big.begin());
}

inline bool end_with(const std::string& big, const std::string& smalls)
{
	return smalls.size() <= big.size()
		&& std::equal(smalls.rbegin(), smalls.rend(), big.rbegin());
}

/// reads a whole line inside a std::string
inline bool mygetline(std::string& l, FILE* fp){
	if(feof(fp)){
		l = {};
		return false;
	}
	std::string buffer;

	while(!feof(fp)) {
		std::string tmp(256, '\1');
		if(fgets(&tmp[0], static_cast<int>(tmp.size()), fp) != nullptr){
			// FIXME: fgets does not return lenght, this will not work if tmp contains embedded '\0'
			const auto len_ = std::find(tmp.begin(), tmp.end(), '\0') - tmp.begin();
			assert(len_ >= 0);
			const auto len = static_cast<std::size_t>(len_);
			if((len > 0) && (tmp.at(len-1) == '\n')){ // read until end of line (what if line ends with '\r')?
				buffer += std::string(tmp, 0, len-1);
				l = buffer;
				return true;
			} else if(len > 0) { // but no terminating line
				buffer += std::string(tmp, 0, len);
			}
		}
	}
	// FIXME: add error checking
	// finished reading file, no eol, but eof --> ok
	l = buffer;
	return !buffer.empty();
}

inline std::vector<std::string> split(const std::string& s, const char del = ' '){
	if(s.empty()){
		return {};
	}
	std::vector<std::string> to_return;

	auto pos_end = s.find(del);
	auto pos_begin = decltype(pos_end){0};

	while(pos_end != std::string::npos){
		to_return.emplace_back(s.substr(pos_begin, pos_end - pos_begin));
		pos_begin = pos_end +1;
		pos_end = s.find(del, pos_begin);
	}
	to_return.emplace_back(s.substr(pos_begin, s.size() - pos_begin));
	return to_return;
}

inline std::vector<std::wstring> split(const std::wstring& s, const wchar_t del = L' ') {
	if (s.empty()) {
		return{};
	}
	std::vector<std::wstring> to_return;

	auto pos_end = s.find(del);
	auto pos_begin = decltype(pos_end){0};

	while (pos_end != std::wstring::npos) {
		to_return.emplace_back(s.substr(pos_begin, pos_end - pos_begin));
		pos_begin = pos_end +1;
		pos_end = s.find(del, pos_begin);
	}
	to_return.emplace_back(s.substr(pos_begin, s.size() - pos_begin));
	return to_return;
}

/// use when converting from char* to std::string and char* may be a nullptr, it will be replaced with an empty string
inline std::string to_string(const char* s){
	return s == nullptr ? "" : s;
}
inline std::wstring to_string(const wchar_t* s) {
	return s == nullptr ? L"" : s;
}


#endif
