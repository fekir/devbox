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

#include "../openssl_memory.hpp"
#include "../hexdump.hpp"

// openssl
#include <openssl/sha.h>
#include <openssl/evp.h>

// std
#include <string>

const std::string hello("Hello World!");

TEST_CASE("md5_hash", "[hash][md5]"){
	const EVP_MD* md = EVP_md5();
	REQUIRE(md != nullptr);

	auto mdctx = make_EVP_MD_CTX_HANDLE(md);
	REQUIRE(mdctx != nullptr);

	const auto res_update = EVP_DigestUpdate(mdctx.get(), hello.c_str(), hello.size());
	REQUIRE(res_update == 1);

	std::vector<unsigned char> md_value(EVP_MAX_MD_SIZE);
	unsigned int md_len = static_cast<unsigned int>(md_value.size());
	const auto res_final = EVP_DigestFinal_ex(mdctx.get(), &md_value[0], &md_len);
	REQUIRE(res_final == 1);
	md_value.resize(md_len);

	const auto d = hexdump::dump(md_value.begin(), md_value.end());
	REQUIRE(d == hexdump::to_dump("ed076287532e86365e841e92bfc50d8c"));
}

TEST_CASE("sha1_hash", "[hash][sha1]"){
	const EVP_MD* md = EVP_sha1();
	REQUIRE(md != nullptr);

	auto mdctx = make_EVP_MD_CTX_HANDLE(md);
	REQUIRE(mdctx != nullptr);

	const auto res_update = EVP_DigestUpdate(mdctx.get(), hello.c_str(), hello.size());
	REQUIRE(res_update == 1);

	std::vector<unsigned char> md_value(EVP_MAX_MD_SIZE);
	unsigned int md_len = static_cast<unsigned int>(md_value.size());
	const auto res_final = EVP_DigestFinal_ex(mdctx.get(), &md_value[0], &md_len);
	REQUIRE(res_final == 1);
	md_value.resize(md_len);

	const auto d = hexdump::dump(md_value.begin(), md_value.end());
	REQUIRE(d == hexdump::to_dump("2ef7bde608ce5404e97d5f042f95f89f1c232871"));
}

TEST_CASE("sha256_hash", "[hash][sha256]"){
	const EVP_MD* md = EVP_sha256();
	REQUIRE(md != nullptr);

	auto mdctx = make_EVP_MD_CTX_HANDLE(md);
	REQUIRE(mdctx != nullptr);

	const auto res_update = EVP_DigestUpdate(mdctx.get(), hello.c_str(), hello.size());
	REQUIRE(res_update == 1);

	std::vector<unsigned char> md_value(EVP_MAX_MD_SIZE);
	unsigned int md_len = static_cast<unsigned int>(md_value.size());
	const auto res_final = EVP_DigestFinal_ex(mdctx.get(), &md_value[0], &md_len);
	REQUIRE(res_final == 1);
	md_value.resize(md_len);

	const auto d = hexdump::dump(md_value.begin(), md_value.end());
	REQUIRE(d == hexdump::to_dump("7f83b1657ff1fc53b92dc18148a1d65dfc2d4b1fa3d677284addd200126d9069"));
}

TEST_CASE("sha512_hash", "[hash][sha512]"){
	const EVP_MD* md = EVP_sha512();
	REQUIRE(md != nullptr);

	auto mdctx = make_EVP_MD_CTX_HANDLE(md);
	REQUIRE(mdctx != nullptr);

	const auto res_update = EVP_DigestUpdate(mdctx.get(), hello.c_str(), hello.size());
	REQUIRE(res_update == 1);

	std::vector<unsigned char> md_value(EVP_MAX_MD_SIZE);
	unsigned int md_len = static_cast<unsigned int>(md_value.size());
	const auto res_final = EVP_DigestFinal_ex(mdctx.get(), &md_value[0], &md_len);
	REQUIRE(res_final == 1);
	md_value.resize(md_len);

	const auto d = hexdump::dump(md_value.begin(), md_value.end());
	REQUIRE(d == hexdump::to_dump("861844d6704e8573fec34d967e20bcfef3d424cf48be04e6dc08f2bd58c729743371015ead891cc3cf1c9d34b49264b510751b1ff9e537937bc46b5d6ff4ecc8"));
}
