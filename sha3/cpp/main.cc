/*
 * =====================================================================================
 *
 *       Filename:  main.cc
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  11/30/2014 05:52:50 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (),
 *   Organization:
 *
 * =====================================================================================
 */


#include <vector>
#include <fstream>
#include <memory>
#include <cstdlib>
#include "sha3.hpp"

template<int HashSize>
char* calculate_sha3(FILE *is, char *digest, char *buff, std::size_t buffsize)
{
    SHA3<HashSize> s;
    while (!std::feof(is)) {
        auto sz = std::fread(buff, 1, buffsize, is);
        s.append(buff, sz);
    }
    s.hex_digest(digest);
    return digest;
}

#ifndef HASH_SIZE
    #define HashSize 256
#else
    #define HashSize HASH_SIZE
#endif
#define BUFFSIZE 150000 * SHA3<HashSize>::rr

int main(int argc, char *argv[]) {
    char digest[HashSize*2+1];
    auto buff = new char[BUFFSIZE + 16];
    auto aligned_buff = buff + (16 - (unsigned long)buff % 16);

    if (argc == 1) {
        calculate_sha3<HashSize>(stdin, digest, aligned_buff, BUFFSIZE);
        std::cout << digest << "\t-\n";
    } else {
        for (int i = 1; i < argc; ++i) {
            FILE *f = std::fopen(argv[i], "rb");
            calculate_sha3<HashSize>(f, digest, aligned_buff, BUFFSIZE);
            std::fclose(f);
            std::cout << digest << "\t" << argv[i] << '\n';
        }
    }

    delete[] buff;
    return 0;
}
