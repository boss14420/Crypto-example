#!/usr/bin/env python3

import array
from functools import partial


class SHA3:
    b = 1600
    w = b // 25
    mask = (1 << w) - 1
    nr = 24

    def __init__(self, output_bits, message=b''):
        self.d = output_bits
        self.c, self.cc = output_bits * 2, output_bits // 4
        self.r = SHA3.b - self.c
        self.rr = self.r // 8
        self._extra_bytes = 0
        self._extra = b''

        self._state = None
        self._digest, self._hex_digest = None, None
        self.reset(message)

    def append(self, message):
        self._absorb(self._extra + message)
        self._calculate_digest()
        return self

    def reset(self, message=b''):
        self._state = [0 for i in range(25)]
        self._absorb(message)
        self._calculate_digest()
        return self

    @staticmethod
    def _array_xor(x, y):
        return array.array('Q', [a ^ b for a, b in zip(x, y)])

    @staticmethod
    def _list_xor(x, y):
        return [a ^ b for a, b in zip(x, y)]

    def _absorb(self, message):
        n = len(message) // self.rr
        self._extra_bytes = len(message) % self.rr
        self._extra = message[n * self.rr:]

        for i in range(n):
            p = array.array('Q', message[i * self.rr:(i + 1) * self.rr] + bytes([0] * self.cc))
            self._state = SHA3.keccak_p(SHA3._list_xor(self._state, p))

    def _calculate_digest(self):
        # pad
        nbytes_pad = (-self._extra_bytes - 1) % self.rr + 1

        extra = bytearray(self._extra)
        if nbytes_pad == 1:
            extra.append(0x86)
        else:
            extra.extend([0x06] + (nbytes_pad - 2) * [0] + [0x80])

        # absorbing
        p = array.array('Q', extra + bytes([0] * self.cc))
        s = SHA3.keccak_p(SHA3._list_xor(self._state, p))

        self._digest = array.array('Q', s).tobytes()[:self.d // 8]
        self._hex_digest = ''.join([format(x, '02x') for x in self._digest])

    def digest(self):
        return self._digest

    def hexdigest(self):
        return self._hex_digest

    # theta
    @staticmethod
    def rot1(x):
        return ((x << 1) | (x >> (SHA3.w - 1))) & SHA3.mask

    @staticmethod
    def rot(x, b):
        return ((x << b) | (x >> (SHA3.w - b))) & SHA3.mask

    # rho constants
    offset = [0, 1, 190, 28, 91,
              36, 300, 6, 55, 276,
              3, 10, 171, 153, 231,
              105, 45, 15, 21, 136,
              210, 66, 253, 120, 78]

    offmodw = [x % (1600 // 25) for x in offset]

    # new positions of lanes in pi method
    divmod5 = lambda a: divmod(a, 5)
    lanesidx = [5 * x + (x + 3 * y) % 5
                for y, x in map(divmod5, range(25))]

    # chi: indexes of 2 next lanes
    n2lanes = [(y * 5 + (x + 1) % 5, y * 5 + (x + 2) % 5)
                   for y, x in map(divmod5, range(25))]

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

    # KECCAK-p
    @staticmethod
    def keccak_p(ss):
        s = ss
        for ir in range(SHA3.nr):
            # theta
            c = [
                s[0] ^ s[5] ^ s[10] ^ s[15] ^ s[20],
                s[1] ^ s[6] ^ s[11] ^ s[16] ^ s[21],
                s[2] ^ s[7] ^ s[12] ^ s[17] ^ s[22],
                s[3] ^ s[8] ^ s[13] ^ s[18] ^ s[23],
                s[4] ^ s[9] ^ s[14] ^ s[19] ^ s[24]]

            d = [
                c[4] ^ SHA3.rot1(c[1]),
                c[0] ^ SHA3.rot1(c[2]),
                c[1] ^ SHA3.rot1(c[3]),
                c[2] ^ SHA3.rot1(c[4]),
                c[3] ^ SHA3.rot1(c[0])
            ]

            s = [s[i] ^ d[i % 5] for i in range(25)]

            # rho
            s = [SHA3.rot(x, b) for x, b in zip(s, SHA3.offmodw)]

            # pi
            s = [s[idx] for idx in SHA3.lanesidx]

            # chi
            s = [s[i] ^ (~s[i1] & s[i2])
                 for (i1, i2), i in zip(SHA3.n2lanes, range(25))]

            # iota
            s[0] ^= (SHA3.rc[ir] & SHA3.mask)

        return array.array('Q', s)


SHA3_224 = partial(SHA3, 224)
SHA3_256 = partial(SHA3, 256)
SHA3_384 = partial(SHA3, 384)
SHA3_512 = partial(SHA3, 512)


# main
def main():
    m = b'abc' * 200
    s = SHA3_224(m)
    print(s.hexdigest())

    s.reset()
    s.append(b'abc' * 100)
    s.append(b'abc' * 49)
    s.append(b'abc' * 51)
    print(s.hexdigest())

    s.reset()
    print(s.hexdigest())


if __name__ == '__main__':
    main()
