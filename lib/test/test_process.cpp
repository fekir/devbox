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

#include "../process.hpp"

#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>

TEST_CASE("echo_", "[process][echo]") {
	const auto pid = ::fork();
	REQUIRE(pid != -1);
	if (pid == 0) {
		exit_on_exit_in_child _(pid);
		execute("/bin/echo", environ_var(), {"a", "b"});
	}
}

TEST_CASE("echo__", "[process][echo]") {
	const auto pid = ::fork();
	REQUIRE(pid != -1);
	if (pid == 0) {
		exit_on_exit_in_child _(pid);
		execute("/bin/echo", environ_var(), std::string("a"), "b");
	}
}

TEST_CASE("echo", "[process][echo]") {
	exec_params p;
	p.waitfinishes = true;
	p.args = {"a", "b c", "c", "d"};
	p.captureoutput = true;

	const auto res = fork_and_execute("/bin/echo", p);
	REQUIRE(res.status == 0);
	REQUIRE(rtrim(res.output) == "a b c c d"); // use rtrim to remove trailing newline
}



