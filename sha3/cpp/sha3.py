#!!/usr/bin/env python3

import ctypes
from ctypes import CDLL
sha3lib = CDLL('./libsha3.so')


class SHA3:
    def __init__(self, hash_size, message=b''):
        self.hash_size = hash_size
        if hash_size == 224:
            self.sha3 = sha3lib.SHA3_224_new(message, len(message))
        elif hash_size == 256:
            self.sha3 = sha3lib.SHA3_256_new(message, len(message))
        elif hash_size == 384:
            self.sha3 = sha3lib.SHA3_384_new(message, len(message))
        elif hash_size == 512:
            self.sha3 = sha3lib.SHA3_512_new(message, len(message))

    def __del__(self):
        if self.hash_size == 224:
            sha3lib.SHA3_224_del(self.sha3)
        elif self.hash_size == 256:
            sha3lib.SHA3_256_del(self.sha3)
        elif self.hash_size == 384:
            sha3lib.SHA3_384_del(self.sha3)
        elif self.hash_size == 512:
            sha3lib.SHA3_512_del(self.sha3)

    def append(self, message):
        if self.hash_size == 224:
            sha3lib.SHA3_224_append(self.sha3, message, len(message))
        elif self.hash_size == 256:
            sha3lib.SHA3_256_append(self.sha3, message, len(message))
        elif self.hash_size == 384:
            sha3lib.SHA3_384_append(self.sha3, message, len(message))
        elif self.hash_size == 512:
            sha3lib.SHA3_512_append(self.sha3, message, len(message))

        return self

    def reset(self, message=b''):
        if self.hash_size == 224:
            sha3lib.SHA3_224_reset(self.sha3, message, len(message))
        elif self.hash_size == 256:
            sha3lib.SHA3_256_reset(self.sha3, message, len(message))
        elif self.hash_size == 384:
            sha3lib.SHA3_384_reset(self.sha3, message, len(message))
        elif self.hash_size == 512:
            sha3lib.SHA3_512_reset(self.sha3, message, len(message))

        return self

    def digest(self):
        dst = ctypes.create_string_buffer(self.hash_size // 8)
        if self.hash_size == 224:
            sha3lib.SHA3_224_digest(self.sha3, dst)
        elif self.hash_size == 256:
            sha3lib.SHA3_256_digest(self.sha3, dst)
        elif self.hash_size == 384:
            sha3lib.SHA3_384_digest(self.sha3, dst)
        elif self.hash_size == 512:
            sha3lib.SHA3_512_digest(self.sha3, dst)

        return dst.value

    def hex_digest(self):
        dst = ctypes.create_string_buffer(self.hash_size // 8 * 2 + 1)
        if self.hash_size == 224:
            sha3lib.SHA3_224_hex_digest(self.sha3, dst)
        elif self.hash_size == 256:
            sha3lib.SHA3_256_hex_digest(self.sha3, dst)
        elif self.hash_size == 384:
            sha3lib.SHA3_384_hex_digest(self.sha3, dst)
        elif self.hash_size == 512:
            sha3lib.SHA3_512_hex_digest(self.sha3, dst)

        return str(dst.value, encoding='ascii')
