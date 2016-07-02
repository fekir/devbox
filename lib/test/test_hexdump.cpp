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

#include "../hexdump.hpp"

#include <string>
#include <vector>
#include <list>

const auto sempty           = std::string("");
const auto semptyhex        = std::string("");
const auto sfoobar1         = std::string("f");
const auto sfoobar1hex      = std::string("66");
const auto sfoobar2         = std::string("fo");
const auto sfoobar2hex      = std::string("666F");
const auto sfoobar3         = std::string("foo");
const auto sfoobar3hex      = std::string("666F6F");
const auto sfoobar4         = std::string("foob");
const auto sfoobar4hex      = std::string("666F6F62");
const auto slorem           = std::string("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Donec a diam lectus. Sed sit amet ipsum mauris. Maecenas congue ligula ac quam viverra nec consectetur ante hendrerit. Donec et mollis dolor. Praesent et diam eget libero egestas mattis sit amet vitae augue. Nam tincidunt congue enim, ut porta lorem lacinia consectetur. Donec ut libero sed arcu vehicula ultricies a non tortor. Lorem ipsum dolor sit amet, consectetur adipiscing elit.");
const auto sloremhex        = std::string("4C6F72656D20697073756D20646F6C6F722073697420616D65742C20636F6E73656374657475722061646970697363696E6720656C69742E20446F6E65632061206469616D206C65637475732E205365642073697420616D657420697073756D206D61757269732E204D616563656E617320636F6E677565206C6967756C61206163207175616D2076697665727261206E656320636F6E736563746574757220616E74652068656E6472657269742E20446F6E6563206574206D6F6C6C697320646F6C6F722E205072616573656E74206574206469616D2065676574206C696265726F2065676573746173206D61747469732073697420616D65742076697461652061756775652E204E616D2074696E636964756E7420636F6E67756520656E696D2C20757420706F727461206C6F72656D206C6163696E696120636F6E73656374657475722E20446F6E6563207574206C696265726F207365642061726375207665686963756C6120756C747269636965732061206E6F6E20746F72746F722E204C6F72656D20697073756D20646F6C6F722073697420616D65742C20636F6E73656374657475722061646970697363696E6720656C69742E");


TEST_CASE("dump_ascii", "[dump][hex][ascii]"){
	REQUIRE(hexdump::dump(sempty) == hexdump::to_dump(semptyhex));
	REQUIRE(hexdump::dump(sfoobar1) == hexdump::to_dump(sfoobar1hex));
	REQUIRE(hexdump::dump(sfoobar2) == hexdump::to_dump(sfoobar2hex));
	REQUIRE(hexdump::dump(sfoobar3) == hexdump::to_dump(sfoobar3hex));
	REQUIRE(hexdump::dump(sfoobar4) == hexdump::to_dump(sfoobar4hex));
	REQUIRE(hexdump::dump(slorem) == hexdump::to_dump(sloremhex));
}


TEST_CASE("dump_binary", "[dump][hex][binary]"){
	std::vector<unsigned char> vec(1000);
	unsigned char c = '\1'; // begin with '\1' and not '\0' or when debugging it's "more difficult" to see the value
	std::generate(vec.begin(), vec.end(), [&c](){return ++c;}); // overflow is OK since unsigned!

	auto d = hexdump::dump(vec.begin(), vec.end());
	auto orig = hexdump::undump(d);

	REQUIRE(orig.size() == 1000);
	for(std::size_t i = 0; i != 1000; ++i){
		REQUIRE(orig.at(i) == vec.at(i));
	}
}

// do not execute normally because it allocates a LOT of memory
TEST_CASE("dump_to_big", "[dump][.]"){
	std::list<unsigned char> l(std::numeric_limits<std::vector<unsigned char>::size_type>::max()/2);
	REQUIRE_THROWS_AS(hexdump::dump(l.begin(), l.end()), std::out_of_range);
}
