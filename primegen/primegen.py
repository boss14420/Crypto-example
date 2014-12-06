#!/usr/bin/env python3

import os
import random
import math


def rabin_miller_test(n, t, s, b):
    """
    test if n = 1 + t * 2**s is strong pseudo-prime base b
    :param n:
    :param t:
    :param s:
    :param b:
    :return:
    """

    b = pow(b, t, n)
    if b == 1 or b == n - 1:
        return True

    for i in range(s - 1):
        b = pow(b, 2, n)
        if b == n - 1:
            return True

    return False


small_primes = [2,   3,   5,   7,  11,  13,  17,  19,  23,  29,  31,  37,  41,
                43,  47,  53,  59,  61,  67,  71]


def prime_test(n, test_num=64):
    # test with small primes
    for p in small_primes:
        if n % p == 0 and n != p:
            return False

    # check whether n is Fermat pseudo-prime base 2
    if pow(2, n - 1, n) != 1:
        return False

    # check whether n is strong pseudo-prime
    t, s = n - 1, 0
    while (t & 1) == 0:
        s += 1
        t >>= 1

    for p in small_primes:
        if not rabin_miller_test(n, t, s, p):
            return False

    for i in range(test_num - len(small_primes)):
        # b = random.randint(small_primes[-1] + 2, n - 1)
        b = random.randint(small_primes[-1] + 2, min((1 << 31) - 1, n - 1))
        if not rabin_miller_test(n, t, s, b):
            return False

    return True


def gen_random_prime(size, test_num=64):
    while True:
        print("\nGenerating random number ...")
        b = os.urandom(size)

        b = bytes([b[0] | 0x80]) + b[1:-1] + bytes([b[-1] | 0x1])
        n = int.from_bytes(b, byteorder='big')
        n = n // 6 * 6 - 1
        for step in range(int(size * 8 * math.log(2)) // 12):
            print("Testing {0} ...\r".format(step), end='')
            if prime_test(n, test_num):
                return n
            if prime_test(n + 2, test_num):
                return n + 2
            n += 6


import sys
def main():
    size = 3072
    if len(sys.argv) > 1:
        size = int(sys.argv[1])
    n = gen_random_prime(size // 8, 64)
    print()
    print(n)


if __name__ == "__main__":
    main()
