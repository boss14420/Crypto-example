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
#include <fstream>
#include <cstdio>

//#define USE_GMP
#include "include/config.hh"
#include "include/oaep.hpp"
#include "include/rsa.hpp"

int main(int argc, char *argv[])
{
//    using namespace oaep;
//    using namespace rsa;

    typedef oaep::HashSHA3<256> Hash;
    typedef oaep::MGF1<Hash> MGF;

//    typedef mpz_class BigInt;

    std::cin.sync_with_stdio(false);
    std::cout.sync_with_stdio(false);

    char mode = argv[1][0];
    if (mode == 'e') {
        rsa::RSAPublicKey<BigInt> publickey;

        // import key
        std::ifstream ifs(argv[2]);
        ifs >> publickey;
        ifs.close();

        // encrypt
        oaep::OAEP<MGF, rsa::RSAPublicKey<BigInt>> oaep(publickey);
        std::size_t keysize = publickey.keysize();
        std::size_t maxMLen = oaep.max_message_len();

        std::vector<unsigned char> input(maxMLen * 10000);
        std::vector<unsigned char> output(keysize / 8 * 10000);

        while (!std::cin.eof()) {
            std::cin.read((char*) input.data(), input.size());
            std::size_t read = std::cin.gcount();
            std::size_t nblocks = 0;

            unsigned char *ip = input.data(), *op = output.data();

            for (; ip + maxMLen <= input.data() + read; ip += maxMLen, op += keysize/8, ++nblocks) {
                oaep.encrypt(ip, maxMLen, op);
            }
            if (ip < input.data() + read) {
                oaep.encrypt(ip, input.data() + read - ip, op);
                ++nblocks;
            }

            std::cout.write((char*) output.data(), nblocks * keysize / 8);
        }

    } else {
        rsa::RSAKey<BigInt> secretkey;

        // import key
        std::ifstream ifs(argv[2]);
        ifs >> secretkey;
        ifs.close();

        oaep::OAEP<MGF, rsa::RSAKey<BigInt>> oaep(secretkey);
        std::size_t keysize = secretkey.keysize();
        std::size_t maxMLen = oaep.max_message_len();

        std::vector<unsigned char> output(maxMLen * 10000);
        std::vector<unsigned char> input(keysize / 8 * 10000);

        while (!std::cin.eof()) {
            std::cin.read((char*) input.data(), input.size());
            std::size_t read = std::cin.gcount();
            std::size_t nblocks = 0;

            unsigned char *ip = input.data(), *op = output.data();

            for (; ip + keysize / 8 <= input.data() + read; ip += keysize / 8, ++nblocks) {
                auto mLen = oaep.decrypt(ip, keysize / 8, op).second;
                op += mLen;
            }

            std::cout.write((char*) output.data(), op - output.data());
        }
    }

    return 0;
}
