/*
 * =====================================================================================
 *
 *       Filename:  primegen.cc
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  12/06/2014 01:29:04 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (),
 *   Organization:
 *
 * =====================================================================================
 */


#include <gmp.h>
#include <gmpxx.h>
#include <iostream>
#include <random>

// n - 1 = t * 2^s
bool rabin_miller_test(mpz_class const &n, mpz_class const &t, int s, mpz_class &b)
{
    mpz_powm(b.get_mpz_t(), b.get_mpz_t(), t.get_mpz_t(), n.get_mpz_t());
    if (b == 1 || b == n - 1) return true;

    for (int i = 0; i < s - 1; ++i) {
        mpz_powm_ui(b.get_mpz_t(), b.get_mpz_t(), 2, n.get_mpz_t());
        if (b == n - 1)
            return true;
    }
    return false;
}

static unsigned small_primes[] = {2,   3,   5,   7,  11,  13,  17,  19,  23,  29,  31,  37,  41,
    43,  47,  53,  59,  61,  67,  71};

bool prime_test(mpz_class const &n, int test_num=40) {
    // test with small primes
    for (auto p : small_primes)
        if (n % p == 0)
            return false;

    // check whether n is Fermat pseudo-prime base 2
    mpz_class tmp;
    mpz_powm(tmp.get_mpz_t(), mpz_class(2).get_mpz_t(),
            mpz_class((n - 1)).get_mpz_t(), n.get_mpz_t());
    if (tmp != 1) return false;

    // check whether n is strong pseudo-prime
    mpz_class t = n - 1; int s = 0;
    while ((t.get_ui() & 1) == 0) {
        ++s; t>>=1;
    }

    for (auto p : small_primes) {
        auto pp = mpz_class(p);
        if (!rabin_miller_test(n, t, s, pp))
            return false;
    }

    int i = sizeof(small_primes)/(sizeof(small_primes[0]));
    std::random_device rd;
    std::default_random_engine e1(rd());
    std::uniform_int_distribution<int>
        dist(small_primes[i-1] + 1, std::numeric_limits<int>::max());
    for (; i < test_num; ++i) {
        auto base = mpz_class(dist(e1));
        if (!rabin_miller_test(n, t, s, base))
            return false;
    }

    return true;
}

#include <cstdio>
#include <chrono>

mpz_class gen_random_prime(std::size_t size, int test_num=40)
{
    std::vector<char> bytearray(size, 0);
    FILE *f = std::fopen("/dev/urandom", "rb");
    mpz_class n;
    while (true) {
        std::fprintf(stderr, "\nGenerating random number ...\n");
        std::fread(&bytearray[0], 1, size, f);
        bytearray[0] |= 0x80; // MSB always 1
        bytearray[size - 1] |= 0x1; // LSB always 1
        mpz_import(n.get_mpz_t(), size, 1, 1, 0, 0, &bytearray[0]);

        auto start = std::chrono::high_resolution_clock::now();
        n = n/6*6 - 1;
        auto max_step = (std::size_t)(size * 8 * std::log2(2.)) / 12;
        for (std::size_t step = 0; step < max_step; ++step) {
            std::fprintf(stderr, "Testing %lu ...\r", step);
            if (prime_test(n, test_num))
                return n;
            if (prime_test(n+2, test_num))
                return n + 2;
            n += 6;
        }
        std::chrono::duration<double> elapsed = std::chrono::high_resolution_clock::now() - start;
        std::fprintf(stderr, "\nAverage test time %fs\n", elapsed.count() / (max_step*2));
    }
    std::fclose(f);
    return 0;
}

int main(int argc, char *argv[])
{
    int size = 3072;
    if (argc > 1) size = std::atoi(argv[1]);
    mpz_class n = gen_random_prime(size / 8, 64);
    std::cout << '\n' << n << '\n';
}
