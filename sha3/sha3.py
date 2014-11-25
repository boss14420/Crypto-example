#!/usr/bin/env python3

import array
import binascii
from functools import partial


class SHA3:
    b = 1600
    w = b // 25
    mask = (1 << w) - 1
    nr = 24

    # rho constants
    offset = [0, 1, 190, 28, 91,
              36, 300, 6, 55, 276,
              3, 10, 171, 153, 231,
              105, 45, 15, 21, 136,
              210, 66, 253, 120, 78]

    offmodw = [x % (1600 // 25) for x in offset]

    def __init__(self, message, output_bits):
        self.d = output_bits
        self.c, self.cc = output_bits * 2, output_bits // 4
        self.r = SHA3.b - self.c
        self.rr = self.r // 8
        self.extra_bytes = 0
        self.extra = b''

        self.state = array.array('Q', [0] * 25)
        self.sponge_next(message)

    @staticmethod
    def array_xor(x, y):
        return array.array('Q', [a ^ b for a, b in zip(x, y)])

    def sponge_next(self, message):
        n = len(message) // self.rr
        self.extra_bytes = len(message) % self.rr
        self.extra = message[n * self.rr:]

        for i in range(n):
            p = array.array('Q', message[i*self.rr:(i+1)*self.rr] + bytes([0] * self.cc))
            self.state = SHA3.keccak_p(SHA3.array_xor(self.state, p))

        return self.state

    def digest(self):
        #  pad
        nbytes_pad = (-self.extra_bytes - 1) % self.rr + 1

        extra = bytearray(self.extra)
        if nbytes_pad == 1:
            extra.append(0x86)
        else:
            extra.extend([0x06] + (nbytes_pad - 2) * [0] + [0x80])

        #  absorbing
        p = array.array('Q', extra + bytes([0] * self.cc))
        s = SHA3.keccak_p(SHA3.array_xor(self.state, p))

        return s.tobytes()[:self.d // 8]

    def hexdigest(self):
        return binascii.hexlify(self.digest())[2:]

    #  KECCAK-p
    @staticmethod
    def keccak_p(s):
        ss = s
        for i in range(SHA3.nr):
            ss = SHA3.theta(ss)
            ss = SHA3.rho(ss)
            ss = SHA3.pi(ss)
            ss = SHA3.chi(ss)
            ss = SHA3.iota(ss, i)

        return ss

    # theta
    @staticmethod
    def rot1(x):
        return ((x << 1) | (x >> (SHA3.w - 1))) & SHA3.mask

    @staticmethod
    def rot(x, b):
        return ((x << b) | (x >> (SHA3.w - b))) & SHA3.mask

    @staticmethod
    def theta(s):
        c = array.array('Q', [
            s[0] ^ s[5] ^ s[10] ^ s[15] ^ s[20],
            s[1] ^ s[6] ^ s[11] ^ s[16] ^ s[21],
            s[2] ^ s[7] ^ s[12] ^ s[17] ^ s[22],
            s[3] ^ s[8] ^ s[13] ^ s[18] ^ s[23],
            s[4] ^ s[9] ^ s[14] ^ s[19] ^ s[24]])

        d = array.array('Q', [
            c[4] ^ SHA3.rot1(c[1]),
            c[0] ^ SHA3.rot1(c[2]),
            c[1] ^ SHA3.rot1(c[3]),
            c[2] ^ SHA3.rot1(c[4]),
            c[3] ^ SHA3.rot1(c[0])
        ])

        return array.array('Q', [s[i] ^ d[i % 5] for i in range(25)])

    #  rho
    @staticmethod
    def rho(s):
        return array.array('Q', [SHA3.rot(x, b)
                                 for x, b in zip(s, SHA3.offmodw)])

    # pi
    _divmod = lambda a, b: divmod(a, b)
    divmod5 = lambda a: divmod(a, 5)
    lanesidx = [5*x + (x+3*y) % 5 for y, x in map(partial(_divmod, b=5), range(25))]

    @staticmethod
    def pi(s):
        return array.array('Q', [s[idx] for idx in SHA3.lanesidx])

    # chi
    @staticmethod
    def chi(s):
        return array.array('Q', [s[i] ^ (~s[i//5*5+(i+1) % 5] & s[i//5*5+(i+2) % 5])
                                 for i in range(25)])

    # iota

    #  Round constants
    rc = [0x0000000000000001, 0x0000000000008082,
          0x800000000000808A, 0x8000000080008000,
          0x000000000000808B, 0x0000000080000001,
          0x8000000080008081, 0x8000000000008009,
          0x000000000000008A, 0x0000000000000088,
          0x0000000080008009, 0x000000008000000A,
          0x000000008000808B, 0x800000000000008B,
          0x8000000000008089, 0x8000000000008003,
          0x8000000000008002, 0x8000000000000080,
          0x000000000000800A, 0x800000008000000A,
          0x8000000080008081, 0x8000000000008080,
          0x0000000080000001, 0x8000000080008008]

    @staticmethod
    def iota(s, ir):
        return array.array('Q', [s[0] ^ (SHA3.rc[ir] & SHA3.mask)]) + s[1:]


SHA3_224 = partial(SHA3, output_bits=224)
SHA3_256 = partial(SHA3, output_bits=256)
SHA3_384 = partial(SHA3, output_bits=384)
SHA3_512 = partial(SHA3, output_bits=512)


# main
def main():
    m = b'abc' * 200
    s = SHA3_224(m)
    print(s.hexdigest())

if __name__ == '__main__':
    main()
