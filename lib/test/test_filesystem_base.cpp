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


#include "../filesystem_utils.hpp"

#include <string>

namespace test{
	const std::string hello("Hello World!");

	TEST_CASE("filesystem path and filename", "[filesystem]"){
		const std::string basepath = "/some/fancy/path/";
		const std::string filename = "filename.txt";

		const std::wstring wbasepath = L"/some/fancy/path/";
		const std::wstring wfilename = L"filename.txt";

		SECTION("get_name"){
			REQUIRE(get_name(basepath+filename) == filename);
		}
		SECTION("get_name_w"){
			REQUIRE(get_name(wbasepath+wfilename) == wfilename);
		}

		SECTION("get_name no directory"){
			REQUIRE(get_name(filename) == filename);
		}
		SECTION("get_name_w no directory"){
			REQUIRE(get_name(wfilename) == wfilename);
		}

		SECTION("get_name no file"){
			REQUIRE(get_name(basepath) == "");
		}
		SECTION("get_name_w no file"){
			REQUIRE(get_name(wbasepath) == L"");
		}

		SECTION("get_path"){
			REQUIRE(get_path(basepath+filename) == basepath);
		}
		SECTION("get_path_w"){
			REQUIRE(get_path(wbasepath+wfilename) == wbasepath);
		}

		SECTION("get_path no directory"){
			REQUIRE(get_path(filename) == "");
		}
		SECTION("get_path_w no directory"){
			REQUIRE(get_path(wfilename) == L"");
		}

		SECTION("get_path no filename"){
			REQUIRE(get_path(basepath) == basepath);
		}
		SECTION("get_path_w no filename"){
			REQUIRE(get_path(wbasepath) == wbasepath);
		}
	}

#if defined(WIN32)

	TEST_CASE("filesystem path and filename windows", "[filesystem]"){
		const std::string basepath = "\\some\\fancy\\path\\";
		const std::string filename = "filename.txt";

		const std::wstring wbasepath = L"\\some\\fancy\\path\\";
		const std::wstring wfilename = L"filename.txt";

		SECTION("get_name"){
			REQUIRE(get_name(basepath+filename) == filename);
		}
		SECTION("get_name_w"){
			REQUIRE(get_name(wbasepath+wfilename) == wfilename);
		}

		SECTION("get_name no directory"){
			REQUIRE(get_name(filename) == filename);
		}
		SECTION("get_name_w no directory"){
			REQUIRE(get_name(wfilename) == wfilename);
		}

		SECTION("get_name no file"){
			REQUIRE(get_name(basepath) == "");
		}
		SECTION("get_name_w no file"){
			REQUIRE(get_name(wbasepath) == L"");
		}

		SECTION("get_path"){
			REQUIRE(get_path(basepath+filename) == basepath);
		}
		SECTION("get_path_w"){
			REQUIRE(get_path(wbasepath+wfilename) == wbasepath);
		}

		SECTION("get_path no directory"){
			REQUIRE(get_path(filename) == "");
		}
		SECTION("get_path_w no directory"){
			REQUIRE(get_path(wfilename) == L"");
		}

		SECTION("get_path no filename"){
			REQUIRE(get_path(basepath) == basepath);
		}
		SECTION("get_path_w no filename"){
			REQUIRE(get_path(wbasepath) == wbasepath);
		}
	}
#endif
}
