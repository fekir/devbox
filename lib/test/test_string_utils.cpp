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


	TEST_CASE("rtrim0", "[string][string_utils][rtrim]"){
		REQUIRE(rtrim(s0) == s0);
	}

	TEST_CASE("rtrim1", "[string][string_utils][rtrim]"){
		REQUIRE(rtrim(s1) == s1);
	}

	TEST_CASE("rtrim2", "[string][string_utils][rtrim]"){
		REQUIRE(rtrim(s2) == "   a");
	}

	TEST_CASE("rtrim3", "[string][string_utils][ltrim]"){
		REQUIRE(rtrim(s3) == s3);
	}

	TEST_CASE("rtrim4", "[string][string_utils][rtrim]"){
		REQUIRE(rtrim(s4) == "a");
	}

	TEST_CASE("rtrim5", "[string][string_utils][rtrim]"){
		REQUIRE(rtrim(s5) == " a");
	}

	TEST_CASE("rtrim6", "[string][string_utils][rtrim]"){
		REQUIRE(rtrim(s6) == "a a");
	}

	TEST_CASE("rtrim7", "[string][string_utils][rtrim]"){
		REQUIRE(rtrim(s7) == "   a a");
	}

	TEST_CASE("rtrim8", "[string][string_utils][rtrim]"){
		REQUIRE(rtrim(s8) == "");
	}

	TEST_CASE("rtrim9", "[string][string_utils][rtrim]"){
		REQUIRE(rtrim(s9) == "");
	}


	TEST_CASE("ltrim0", "[string][string_utils][ltrim]"){
		REQUIRE(ltrim(s0) == "a");
	}

	TEST_CASE("ltrim1", "[string][string_utils][ltrim]"){
		REQUIRE(ltrim(s1) == "a");
	}

	TEST_CASE("ltrim2", "[string][string_utils][ltrim]"){
		REQUIRE(ltrim(s2) == "a ");
	}

	TEST_CASE("ltrim3", "[string][string_utils][ltrim]"){
		REQUIRE(ltrim(s3) == "a a");
	}

	TEST_CASE("ltrim4", "[string][string_utils][ltrim]"){
		REQUIRE(ltrim(s4) == s4);
	}

	TEST_CASE("ltrim5", "[string][string_utils][ltrim]"){
		REQUIRE(ltrim(s5) == "a   ");
	}

	TEST_CASE("ltrim6", "[string][string_utils][ltrim]"){
		REQUIRE(ltrim(s6) == s6);
	}

	TEST_CASE("ltrim7", "[string][string_utils][ltrim]"){
		REQUIRE(ltrim(s7) == "a a   ");
	}

	TEST_CASE("ltrim8", "[string][string_utils][ltrim]"){
		REQUIRE(ltrim(s8) == "");
	}

	TEST_CASE("ltrim9", "[string][string_utils][ltrim]"){
		REQUIRE(ltrim(s9) == "");
	}



	TEST_CASE("trim0", "[string][string_utils][trim]"){
		REQUIRE(trim(s0) == ltrim(rtrim(s0)));
		REQUIRE(trim(s0) == rtrim(ltrim(s0)));
	}

	TEST_CASE("trim1", "[string][string_utils][trim]"){
		REQUIRE(trim(s1) == ltrim(rtrim(s1)));
		REQUIRE(trim(s1) == rtrim(ltrim(s1)));
	}

	TEST_CASE("trim2", "[string][string_utils][trim]"){
		REQUIRE(trim(s2) == ltrim(rtrim(s2)));
		REQUIRE(trim(s2) == rtrim(ltrim(s2)));
	}

	TEST_CASE("trim3", "[string][string_utils][trim]"){
		REQUIRE(trim(s3) == ltrim(rtrim(s3)));
		REQUIRE(trim(s3) == rtrim(ltrim(s3)));
	}

	TEST_CASE("trim4", "[string][string_utils][trim]"){
		REQUIRE(trim(s4) == ltrim(rtrim(s4)));
		REQUIRE(trim(s4) == rtrim(ltrim(s4)));
	}

	TEST_CASE("trim5", "[string][string_utils][trim]"){
		REQUIRE(trim(s5) == ltrim(rtrim(s5)));
		REQUIRE(trim(s5) == rtrim(ltrim(s5)));
	}

	TEST_CASE("trim6", "[string][string_utils][trim]"){
		REQUIRE(trim(s6) == ltrim(rtrim(s6)));
		REQUIRE(trim(s6) == rtrim(ltrim(s6)));
	}

	TEST_CASE("trim7", "[string][string_utils][trim]"){
		REQUIRE(trim(s7) == ltrim(rtrim(s7)));
		REQUIRE(trim(s7) == rtrim(ltrim(s7)));
	}

	TEST_CASE("trim8", "[string][string_utils][trim]"){
		REQUIRE(trim(s8) == ltrim(rtrim(s8)));
		REQUIRE(trim(s8) == rtrim(ltrim(s8)));
	}

	TEST_CASE("trim9", "[string][string_utils][trim]"){
		REQUIRE(trim(s9) == ltrim(rtrim(s9)));
		REQUIRE(trim(s9) == rtrim(ltrim(s9)));
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
