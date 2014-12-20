#!/usr/bin/env python3

import math


def prime_gen(size):
    return 1


def invmod(a, m):
    s1, s0 = 0, 1
    # t1, t0 = 1, 0
    r1, r0 = m, a
    while r1 != 0:
        q, r = divmod(r0, r1)
        r0, r1 = r1, r
        s0, s1 = s1, s0 - q * s1
        # t0, t1 = t1, t0 - q * t1
    while s0 < 0:
        s0 += m
    return s0


class RSAPublicKey:
    def __init__(self, n, e, keysize):
        self.n, self.e = n, e
        self._keysize = keysize

    def modulus(self):
        return self.n

    def exponent(self):
        return self.e

    def keysize(self):
        return self._keysize

    def encrypt_int(self, m):
        return pow(m, self.e, self.n)

    def encrypt_bytes(self, mbytes, output_size):
        m = int.from_bytes(mbytes, byteorder='big')
        c = self.encrypt_int(m)
        cbytes = int.to_bytes(c, output_size, byteorder='big')
        return cbytes


class RSAKey(RSAPublicKey):
    def __init__(self, p, q):
        n = p * q
        self.phi = (p - 1) * (q - 1)
        e = 65537
        self.d = invmod(e, self.phi)
        self.dp = self.d % (p - 1)
        self.dq = self.d % (q - 1)
        self.qinv = invmod(q, p)
        self.p, self.q = p, q keysize = int(math.log(n, 256)) + 1
        super().__init__(n, e, keysize)

    def public_key(self):
        return RSAPublicKey(self.n, self.e, self._keysize)

    def decrypt_int(self, c):
        m1 = pow(c, self.dp, self.p)
        m2 = pow(c, self.dq, self.q)
        h = (self.qinv * (m1 - m2)) % self.p
        m = m2 + self.q * h
        return m

    def decrypt_bytes(self, cbytes, output_size):
        c = int.from_bytes(cbytes, byteorder='big')
        m = self.decrypt_int(c)
        mbytes = int.to_bytes(m, output_size, byteorder='big')
        return mbytes


class RSA:
    @staticmethod
    def generate(keysize):
        # p = prime_gen(keysize // 2)
        # q = 0
        # while q == p:
        #     q = prime_gen(keysize // 2)
        p, q = 13, 17
        return RSAKey(p, q)
