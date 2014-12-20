/*
 * =====================================================================================
 *
 *       Filename:  rsatest.cc
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  12/20/2014 03:33:30 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (),
 *   Organization:
 *
 * =====================================================================================
 */

#include <cstdio>
#include <cstdlib>
#include <iostream>

//#define USE_BOOST_MP
#define USE_GMP

//#include <boost/multiprecision/cpp_int.hpp>
//#include <boost/multiprecision/gmp.hpp>

#include "bigint.hpp"
#include "rsa.hpp"

int main(int argc, char *argv[]) {
//    typedef boost::multiprecision::cpp_int Int;
//    typedef boost::multiprecision::mpz_int Int;
    typedef mpz_class Int;
    auto key = rsa::generate_rsakey<Int>(6144);
    auto public_key = key.public_key();

    std::cout << "n = " << key.modulus() << '\n';
    std::cout << "e = " << key.exponent() << '\n';

    Int m = 13;
    if (argc > 1) m = std::atoi(argv[1]);

    auto c = public_key.encrypt_int(m);
    std::cout << c << '\n';
    m = key.decrypt_int(c);
    std::cout << m << '\n';

    return 0;
}
