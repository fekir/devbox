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
#include "utils.hpp"


// glib
#include <glib.h> // GList

// posix
#include <fnmatch.h> // fnmatch
#include <unistd.h>  // execve

// std
#include <iostream>
#include <functional>
#include <algorithm>


extern char** environ;

namespace test{

	TEST_CASE("filematcher", ""){
		static_assert(FNM_NOMATCH != 0, "unable to detect error");
		REQUIRE(fnmatch("*", "hello.txt", 0) == 0);
		REQUIRE(fnmatch("*.txt", "hello.txt", 0) == 0);
		REQUIRE(fnmatch("*.txt2", "hello.txt", 0) == FNM_NOMATCH);
		//REQUIRE(fnmatch("*.txt || *.txt2", "hello.txt", 0) == 0); // --> does not work!!!
		//REQUIRE(fnmatch("*.{txt,txt2}", "hello.txt", 0) == 0);
		REQUIRE(fnmatch("hello.txt", "hello.txt", 0) == 0);
	}

	TEST_CASE("consoleprogram", "[.]"){
		std::string console = "/usr/bin/mate-terminal";
		std::string program_and_params = "/bin/ls -la";
		std::string param1 = "-e";
		//	std::string param2 = "/bin/bash -c 'echo executing "+ program_and_params + ";\n"+program_and_params+";echo finshed; exec /bin/bash '";
		std::string param2 = create_command_for_console(program_and_params);

		char* const args[] = {&console[0], &param1[0], &param2[0], nullptr};
		//char* const env[] = {&display[0], nullptr};
		auto res = execve(args[0], args, environ); // check != -1
		REQUIRE(res != -1);
	}

	TEST_CASE("guiprogram", "[.]"){
		//std::string display = "DISPLAY=" + get_env("DISPLAY");
		std::string cmake_gui = "/usr/bin/cmake-gui";

		char* const args[] = {&cmake_gui[0], nullptr};
		//char* const env[] = {&display[0], nullptr};
		const pid_t child_pid = fork();
		if(child_pid == 0){ // in child
			const auto res = execve(args[0], args, environ);
			REQUIRE(execve(args[0], args, environ) != -1);
		}
	}

	TEST_CASE("emptydirname", "[]"){
		REQUIRE(dir_contains("/", {"tmp"}));
		REQUIRE(!dir_contains("", {"tmp"}));
	}


	TEST_CASE("match_with_bind", "[match]"){
		using namespace std::placeholders;
		const std::string filename = "testfile.cpp";
		const auto match_against_filename = std::bind(file_match, _1, filename);
		const auto res = std::any_of(c_cpp_files.begin(), c_cpp_files.end(), match_against_filename);
		REQUIRE(res);
	}


	TEST_CASE("getpath", "[path]"){
		auto path = get_env_path();

		// FIXME: not very robust as test, but better than nothing
		REQUIRE(std::find(path.begin(), path.end(), "/usr/bin") != path.end());
		REQUIRE(std::find(path.begin(), path.end(), "/usr/local/bin") != path.end());
		REQUIRE(std::find(path.begin(), path.end(), "/bin") != path.end());
	}

}
