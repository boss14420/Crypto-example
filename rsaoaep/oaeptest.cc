/*
 * =====================================================================================
 *
 *       Filename:  oaeptest.cc
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  12/21/2014 12:24:28 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (),
 *   Organization:
 *
 * =====================================================================================
 */


#include <iostream>
#include <cstdio>

#define USE_GMP
#include "oaep.hpp"
#include "rsa.hpp"

int main()
{
    unsigned char message[] = "12345";
    std::size_t keysize = 1024;
    std::vector<unsigned char> cipher(keysize / 8);

    using namespace oaep;
    using namespace rsa;

    typedef HashSHA3<256> Hash;
    typedef MGF1<Hash> MGF;

    typedef mpz_class Int;
    RSAKey<Int> key = generate_rsakey<Int>(keysize);

    OAEP<MGF, RSAKey<Int>> oaep(key);
    oaep.encrypt(message, 5, cipher.data());

    for (auto b : cipher)
        std::printf("%02x", b);
    std::putchar('\n');


    std::vector<unsigned char> message2(keysize / 8, 0);
    auto mLen = oaep.decrypt(cipher.data(), keysize / 8, message2.data()).second;

    for (auto i = 0LU; i != mLen; ++i)
        std::printf("%02x", message2[i]);
    std::putchar('\n');

}
