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

#ifndef H_OPENSSL_MEMORY
#define H_OPENSSL_MEMORY

// openssl
 #include <openssl/evp.h>

// std
#include <memory>

struct EVP_MD_CTX_FREE_S{
	void operator()(EVP_MD_CTX* ctx) const {
#if OPENSSL_VERSION_NUMBER < 0x10100000 // EVP_MD_CTX_free should be avaiable from version 1.1
		EVP_MD_CTX_destroy( ctx);
#else
		EVP_MD_CTX_free(ctx);
#endif
	}
};
using EVP_MD_CTX_HANDLE = std::unique_ptr<EVP_MD_CTX, EVP_MD_CTX_FREE_S>;
inline EVP_MD_CTX_HANDLE make_EVP_MD_CTX_HANDLE(){
#if OPENSSL_VERSION_NUMBER < 0x10100000 // EVP_MD_CTX_new should be avaiable from version 1.1
	    EVP_MD_CTX_HANDLE mdctx(EVP_MD_CTX_create());
#else
	    EVP_MD_CTX_HANDLE mdctx(EVP_MD_CTX_new());
#endif
		return mdctx;
}
/// returns an initialized EVP_MD_CTX_HANDLE
inline EVP_MD_CTX_HANDLE make_EVP_MD_CTX_HANDLE(const EVP_MD* md){
	auto mdctx = make_EVP_MD_CTX_HANDLE();
	const int res = EVP_DigestInit_ex(mdctx.get(), md, nullptr);
	if(res != 1){
		throw std::runtime_error("unable to initialize EVP");
	}
	return mdctx;
}

inline std::string to_string(const EVP_MD* md){
	const char* name = EVP_MD_name(md);
	return name == nullptr ? "" : name;
}

#endif
