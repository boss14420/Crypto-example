#!/usr/bin/env python3

###########################################3

lxor = lambda X, Y: [x^y for x,y in zip(X, Y)]
bxor = lambda X, Y: bytes(lxor(X, Y))

###################################################
########### CBC
###################################################

def cbc_encrypt(plaintext, key, encrypt_func, block_len, iv):
    l = len(plaintext)

    if l % block_len != 0:
        raise ValueError("Input strings must be a multiple of 16 in length")

    ciphertext = bytearray()
    c0 = iv

    #q, r = divmod(l, block_len)
    for i in range(0, l, block_len):
        m = bxor(c0, plaintext[i:i+block_len])
        c0 = encrypt_func(m, key)
        ciphertext += c0

    return ciphertext


def cbc_decrypt(ciphertext, key, decrypt_func, block_len, iv):
    l = len(ciphertext)

    if l % block_len != 0:
        raise ValueError("Input strings must be a multiple of 16 in length")

    plaintext = bytearray()

    #TODO: paralleilize
    c0 , c1 , i = iv, None, 0
    for i in range(0, l, block_len):
        c0, c1 = ciphertext[i:i+block_len], c0
        m = decrypt_func(c0, key)
        m = bxor(c1, m)
        plaintext += m

    return plaintext

###################################################
########### CTR
###################################################

def ctr_encrypt(plaintext, key, encrypt_func, block_len, counter):
    l = len(plaintext)
    ciphertext = bytearray()

    # TODO: parallelize
    for i in range(0, l, block_len):
        #m = iv + (b).to_bytes(l2, byteorder='big')
        c0 = encrypt_func(counter(), key)
        c0 = bxor(c0, plaintext[i:i+block_len])
        ciphertext += c0

    return ciphertext


def ctr_decrypt(ciphertext, key, encrypt_func, block_len, counter):
    l = len(ciphertext)
    plaintext = bytearray()

    # TODO: parallelize
    for i in range(0, l, block_len):
        m = encrypt_func(counter(), key)
        m = bxor(m, ciphertext[i:i+block_len])
        plaintext += m

    return plaintext

#############
