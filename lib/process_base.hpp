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

/// This package contains utility functions for invoking programs

#ifndef H_PROCESS_BASE_0
#define H_PROCESS_BASE_0

#include <string>
#include <vector>
#include <exception>

// converts the vector of argument and program name to a vector that can be used as argv.
// Notice that argv needs to be "char* const", and vector<const char*> would be "const char* const"
inline std::vector<char*> to_argv(std::string& program, std::vector<std::string>& arguments){
	std::vector<char*> toreturn; toreturn.reserve(arguments.size()+2);
	toreturn.push_back(&program[0]);
	for(auto& arg : arguments) {
		toreturn.push_back(&arg[0]);
	}
	toreturn.push_back(nullptr);
	return toreturn;
}

inline std::vector<wchar_t*> to_argv(std::wstring& program, std::vector<std::wstring>& arguments){
	std::vector<wchar_t*> toreturn; toreturn.reserve(arguments.size()+2);
	toreturn.push_back(&program[0]);
	for(auto& arg : arguments) {
		toreturn.push_back(&arg[0]);
	}
	toreturn.push_back(nullptr);
	return toreturn;
}

inline std::vector<char*> to_argv(std::vector<std::string>& arguments){
	std::vector<char*> toreturn; 
	toreturn.reserve(arguments.size()+1);
	for(auto& arg : arguments) {
		toreturn.push_back(&arg[0]);
	}
	toreturn.push_back(nullptr);
	return toreturn;
}

inline std::vector<wchar_t*> to_argv(std::vector<std::wstring>& arguments){
	std::vector<wchar_t*> toreturn;
	toreturn.reserve(arguments.size()+1);
	for(auto& arg : arguments) {
		toreturn.push_back(&arg[0]);
	}
	toreturn.push_back(nullptr);
	return toreturn;
}

// warning: not really type safe
inline std::string args_to_str(const std::vector<std::string>& ss, bool parse = false) {
	std::string toreturn;
	if (parse) {
		for (const auto& s : ss) {
			toreturn += "\"" + s + "\" ";
		}
	}
	else {
		for (const auto& s : ss) {
			toreturn += s + ' ';
		}
	}
	return toreturn;
}

// warning: not really type safe
inline std::wstring args_to_str(const std::vector<std::wstring>& ss, bool parse = false) {
	std::wstring toreturn;
	for (auto it = ss.begin(); it != ss.end(); ++it) {
		if (parse && it->find(L" ") != std::wstring::npos) {
			toreturn += L"\"" + *it +L"\"";
			if (it + 1 != ss.end()) {
				toreturn+=L" ";
			}
		} else {
			toreturn += *it;
			if (it + 1 != ss.end()) {
				toreturn += L" ";
			}
		}
	}
	return toreturn;
}

/// use struct instead of char* const*, for avoiding confusion between env and args
struct environ_var{
	char* const* envp = nullptr;
	constexpr explicit environ_var(char* const envp_[]) : envp(envp_){}
	constexpr explicit environ_var(){}
	char* const* getenv() const {return envp;}
};

struct wenviron_var{
	wchar_t* const* envp = nullptr;
	constexpr explicit wenviron_var(wchar_t* const envp_[]) : envp(envp_){}
	constexpr explicit wenviron_var(){}
	wchar_t* const* getenv() const {return envp;}
};

#endif
