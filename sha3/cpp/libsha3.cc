/*
 * =====================================================================================
 *
 *       Filename:  sha3.cc
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  12/04/2014 07:10:35 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (),
 *   Organization:
 *
 * =====================================================================================
 */


#include "sha3.hpp"

typedef SHA3<224> SHA3_224;
typedef SHA3<256> SHA3_256;
typedef SHA3<384> SHA3_384;
typedef SHA3<512> SHA3_512;

extern "C" {

#define DEF_C_FUNCTIONS(HASHSIZE) \
SHA3_##HASHSIZE* SHA3_##HASHSIZE##_new(char const *message = nullptr, std::size_t len = 0) { \
    return new SHA3_##HASHSIZE(message, len); \
} \
\
SHA3_##HASHSIZE* SHA3_##HASHSIZE##_append(SHA3_##HASHSIZE* sha3, char const* message, std::size_t len) { \
    sha3->append(message, len); \
    return sha3; \
} \
\
SHA3_##HASHSIZE* SHA3_##HASHSIZE##_reset(SHA3_##HASHSIZE* sha3, char const* message = nullptr, std::size_t len=0) { \
    sha3->reset(message, len); \
    return sha3; \
} \
 \
char *SHA3_##HASHSIZE##_hex_digest(SHA3_##HASHSIZE const* sha3, char *digest) { \
    return sha3->hex_digest(digest); \
} \
 \
char *SHA3_##HASHSIZE##_digest(SHA3_##HASHSIZE const* sha3, char *digest) { \
    return sha3->digest(digest); \
} \
 \
void SHA3_##HASHSIZE##_del(SHA3_##HASHSIZE* sha3) { \
    delete sha3; \
}

DEF_C_FUNCTIONS(224)
DEF_C_FUNCTIONS(256)
DEF_C_FUNCTIONS(384)
DEF_C_FUNCTIONS(512)
}
