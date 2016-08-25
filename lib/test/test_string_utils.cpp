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
#include "string_utils.hpp"

// std
#include <string>
#include <iostream>

namespace test{

	const std::string s0 = "a";

	const std::string s1 = "   a";
	const std::string s2 = "   a ";
	const std::string s3 = "   a a";

	const std::string s4 = "a   ";
	const std::string s5 = " a   ";
	const std::string s6 = "a a   ";


	const std::string s7 = "   a a   ";
	const std::string s8 = "         ";
	const std::string s9 = "";

	TEST_CASE("rtrim", "[string][string_utils][rtrim]") {

		SECTION("s0") {
			REQUIRE(rtrim(s0) == s0);
		}

		SECTION("s1") {
			REQUIRE(rtrim(s1) == s1);
		}

		SECTION("s2") {
			REQUIRE(rtrim(s2) == "   a");
		}

		SECTION("s3") {
			REQUIRE(rtrim(s3) == s3);
		}

		SECTION("s4") {
			REQUIRE(rtrim(s4) == "a");
		}

		SECTION("s5") {
			REQUIRE(rtrim(s5) == " a");
		}

		SECTION("s6") {
			REQUIRE(rtrim(s6) == "a a");
		}

		SECTION("s7") {
			REQUIRE(rtrim(s7) == "   a a");
		}

		SECTION("s8") {
			REQUIRE(rtrim(s8) == "");
		}

		SECTION("s9") {
			REQUIRE(rtrim(s9) == "");
		}
	}

	TEST_CASE("ltrim", "[string][string_utils][rtrim]") {
		SECTION("s0") {
			REQUIRE(ltrim(s0) == "a");
		}

		SECTION("s1") {
			REQUIRE(ltrim(s1) == "a");
		}

		SECTION("s2") {
			REQUIRE(ltrim(s2) == "a ");
		}

		SECTION("s3") {
			REQUIRE(ltrim(s3) == "a a");
		}

		SECTION("s4") {
			REQUIRE(ltrim(s4) == s4);
		}

		SECTION("s5") {
			REQUIRE(ltrim(s5) == "a   ");
		}

		SECTION("s6") {
			REQUIRE(ltrim(s6) == s6);
		}

		SECTION("s7") {
			REQUIRE(ltrim(s7) == "a a   ");
		}

		SECTION("s8") {
			REQUIRE(ltrim(s8) == "");
		}

		SECTION("s9") {
			REQUIRE(ltrim(s9) == "");
		}
	}


	TEST_CASE("trim", "[string][string_utils][trim]") {
		SECTION("s0") {
			REQUIRE(trim(s0) == ltrim(rtrim(s0)));
			REQUIRE(trim(s0) == rtrim(ltrim(s0)));
		}

		SECTION("s1") {
			REQUIRE(trim(s1) == ltrim(rtrim(s1)));
			REQUIRE(trim(s1) == rtrim(ltrim(s1)));
		}

		SECTION("s2") {
			REQUIRE(trim(s2) == ltrim(rtrim(s2)));
			REQUIRE(trim(s2) == rtrim(ltrim(s2)));
		}

		SECTION("s3") {
			REQUIRE(trim(s3) == ltrim(rtrim(s3)));
			REQUIRE(trim(s3) == rtrim(ltrim(s3)));
		}

		SECTION("s4") {
			REQUIRE(trim(s4) == ltrim(rtrim(s4)));
			REQUIRE(trim(s4) == rtrim(ltrim(s4)));
		}

		SECTION("s5") {
			REQUIRE(trim(s5) == ltrim(rtrim(s5)));
			REQUIRE(trim(s5) == rtrim(ltrim(s5)));
		}

		SECTION("s6") {
			REQUIRE(trim(s6) == ltrim(rtrim(s6)));
			REQUIRE(trim(s6) == rtrim(ltrim(s6)));
		}

		SECTION("s7") {
			REQUIRE(trim(s7) == ltrim(rtrim(s7)));
			REQUIRE(trim(s7) == rtrim(ltrim(s7)));
		}

		SECTION("s8") {
			REQUIRE(trim(s8) == ltrim(rtrim(s8)));
			REQUIRE(trim(s8) == rtrim(ltrim(s8)));
		}

		SECTION("s9") {
			REQUIRE(trim(s9) == ltrim(rtrim(s9)));
			REQUIRE(trim(s9) == rtrim(ltrim(s9)));
		}
	}


	TEST_CASE("split", "[split]"){
		const std::string to_split = "a,bb,ccc,dddd,eee,ff,g,,h";
		const auto res = split(to_split, ',');
		REQUIRE(res.size() == 9);
		REQUIRE(res[0] == "a");
		REQUIRE(res[1] == "bb");
		REQUIRE(res[2] == "ccc");
		REQUIRE(res[3] == "dddd");
		REQUIRE(res[4] == "eee");
		REQUIRE(res[5] == "ff");
		REQUIRE(res[6] == "g");
		REQUIRE(res[7] == "");
		REQUIRE(res[8] == "h");
	}

}
