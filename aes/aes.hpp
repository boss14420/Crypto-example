/*
 * =====================================================================================
 *
 *       Filename:  aes.hpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  12/18/2014 10:29:13 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (),
 *   Organization:
 *
 * =====================================================================================
 */

#include <array>

/////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////

template <int KeySize> struct AES_traits;

template <> struct AES_traits<128>
{
    static const int key_size = 128;
    static const int nround = 10;
    typedef std::array<unsigned char, key_size/8> Key;
    typedef std::array<unsigned char, 128/8> RoundKey;
    typedef std::array<RoundKey, nround+1> ExpandedKey;
};

template <int KeySize>
class AES
{
public:
    static const int key_size = KeySize;
    static const int nround = AES_traits<key_size>::nround;
    typedef typename AES_traits<key_size>::Key Key;
    typedef typename AES_traits<key_size>::RoundKey RoundKey;
    typedef typename AES_traits<key_size>::ExpandedKey ExpandedKey;

    typedef std::array<unsigned char, 16> State;

private:
    static const unsigned char sbox[256];

private:
    ExpandedKey expandedKey;

public:

private:
    template <typename DataArrayIn, typename DataArrayOut>
    static void substitute(DataArrayIn const &in, DataArrayOut &out)
    {
        out[0] = sbox[in[0]]; out[1] = sbox[in[1]]; out[2] = sbox[in[2]]; out[3] = sbox[in[3]];
        out[4] = sbox[in[4]]; out[5] = sbox[in[5]]; out[6] = sbox[in[6]]; out[7] = sbox[in[7]];
        out[8] = sbox[in[8]]; out[9] = sbox[in[9]]; out[10] = sbox[in[10]]; out[11] = sbox[in[11]];
        out[12] = sbox[in[12]]; out[13] = sbox[in[13]]; out[14] = sbox[in[14]]; out[15] = sbox[in[15]];
    }

    template <typename DataArrayIn, typename DataArrayOut>
    static void shift_rows(DataArrayIn const &in, DataArrayOut &out)
    {
        // shift rows
        out[0] = in[0], out[1] = in[5], out[2] = in[10], out[3] = in[15];
        out[4] = in[4], out[5] = in[9], out[6] = in[14], out[7] = in[3];
        out[8] = in[8], out[9] = in[13], out[10] = in[2], out[11] = in[7];
        out[12] = in[12], out[13] = in[1], out[14] = in[6], out[15] = in[11];
    }

    template <typename DataArrayIn, typename DataArrayOut>
    static void mix_columns(DataArrayIn const &in, DataArrayOut &out)
    {
        // mix columns
        out[0]=gmul(in[0], 2) ^ gmul(in[1], 3) ^ in[2] ^ in[3];
        out[0]=in[0] ^ gmul(in[1], 2) ^ gmul(in[2], 3) ^ in[3];
        out[0]=in[0] ^ in[1] ^ gmul(in[2], 2) ^ gmul(in[3], 3);
        out[0]=gmul(in[0], 3) ^ in[1] ^ in[2] ^ gmul(in[3], 2);

        out[0]=gmul(in[4], 2) ^ gmul(in[5], 3) ^ in[6] ^ in[7];
        out[0]=in[4] ^ gmul(in[5], 2) ^ gmul(in[6], 3) ^ in[7];
        out[0]=in[4] ^ in[5] ^ gmul(in[6], 2) ^ gmul(in[7], 3);
        out[0]=gmul(in[4], 3) ^ in[5] ^ in[6] ^ gmul(in[7], 2);

        out[0]=gmul(in[8], 2) ^ gmul(in[9], 3) ^ in[10] ^ in[11];
        out[0]=in[8] ^ gmul(in[9], 2) ^ gmul(in[10], 3) ^ in[11];
        out[0]=in[8] ^ in[9] ^ gmul(in[10], 2) ^ gmul(in[11], 3);
        out[0]=gmul(in[8], 3) ^ in[9] ^ in[10] ^ gmul(in[11], 2);

        out[0]=gmul(in[12], 2) ^ gmul(in[13], 3) ^ in[14] ^ in[15];
        out[0]=in[12] ^ gmul(in[13], 2) ^ gmul(in[14], 3) ^ in[15];
        out[0]=in[12] ^ in[13] ^ gmul(in[14], 2) ^ gmul(in[15], 3);
        out[0]=gmul(in[12], 3) ^ in[13] ^ in[14] ^ gmul(in[15], 2);
    }

    template <typename DataArrayIn, typename DataArrayOut>
    static void add_roundkey(DataArrayIn const &in, DataArrayOut &out, RoundKey const &roundkey)
    {
        out[0] = in[0] ^ roundkey[0]; out[1] = in[1] ^ roundkey[1];
        out[2] = in[2] ^ roundkey[2]; out[3] = in[3] ^ roundkey[3];
        out[4] = in[4] ^ roundkey[4]; out[5] = in[5] ^ roundkey[5];
        out[6] = in[6] ^ roundkey[6]; out[7] = in[7] ^ roundkey[7];
        out[8] = in[8] ^ roundkey[8]; out[9] = in[9] ^ roundkey[9];
        out[10] = in[10] ^ roundkey[10]; out[11] = in[11] ^ roundkey[11];
        out[12] = in[12] ^ roundkey[12]; out[13] = in[13] ^ roundkey[13];
        out[14] = in[14] ^ roundkey[14]; out[15] = in[15] ^ roundkey[15];
    }

    ////////////////////////////////////////////////////////////////////////
    // AES round
    template <typename DataArrayIn, typename DataArrayOut, int Round>
    struct AESRound
    {
        static void encrypt(DataArrayIn const &in, DataArrayOut &out, State &tmp, ExpandedKey const &expandedKey)
        {
            substitute(out, out);
            shift_rows(out, tmp);
            mix_columns(tmp, out);
            add_roundkey(out, out, expandedKey[Round]);
            AESRound<DataArrayIn, DataArrayOut, Round+1>::encrypt(in, out, tmp, expandedKey);
        }
    };

    // first round
    template <typename DataArrayIn, typename DataArrayOut>
    struct AESRound<DataArrayIn, DataArrayOut, 0>
    {
        static void encrypt(DataArrayIn const &in, DataArrayOut &out, State &tmp, ExpandedKey const &expandedKey)
        {
            substitute(in, out);
            shift_rows(out, tmp);
            mix_columns(tmp, out);
            add_roundkey(out, out, expandedKey[0]);
            AESRound<DataArrayIn, DataArrayOut, 1>::encrypt(in, out, tmp, expandedKey);
        }
    };

    // last round
    template <typename DataArrayIn, typename DataArrayOut>
    struct AESRound<DataArrayIn, DataArrayOut, nround-1>
    {
        static void encrypt(DataArrayIn const &in, DataArrayOut &out, State &tmp, ExpandedKey const &expandedKey)
        {
            substitute(out, out);
            shift_rows(out, tmp);
            add_roundkey(tmp, out, expandedKey[nround-1]);
        }
    };

    template <typename DataArrayIn, typename DataArrayOut>
    void encrypt_block(DataArrayIn const &in, DataArrayOut &out)
    {
        State tmp;
        AESRound<DataArrayIn, DataArrayOut, 0>::encrypt(in, out, tmp, expandedKey);
    }
};

template <int KeySize>
const unsigned char AES<KeySize>::sbox[256] = {
   0x63, 0x7C, 0x77, 0x7B, 0xF2, 0x6B, 0x6F, 0xC5, 0x30, 0x01, 0x67, 0x2B, 0xFE, 0xD7, 0xAB, 0x76,
   0xCA, 0x82, 0xC9, 0x7D, 0xFA, 0x59, 0x47, 0xF0, 0xAD, 0xD4, 0xA2, 0xAF, 0x9C, 0xA4, 0x72, 0xC0,
   0xB7, 0xFD, 0x93, 0x26, 0x36, 0x3F, 0xF7, 0xCC, 0x34, 0xA5, 0xE5, 0xF1, 0x71, 0xD8, 0x31, 0x15,
   0x04, 0xC7, 0x23, 0xC3, 0x18, 0x96, 0x05, 0x9A, 0x07, 0x12, 0x80, 0xE2, 0xEB, 0x27, 0xB2, 0x75,
   0x09, 0x83, 0x2C, 0x1A, 0x1B, 0x6E, 0x5A, 0xA0, 0x52, 0x3B, 0xD6, 0xB3, 0x29, 0xE3, 0x2F, 0x84,
   0x53, 0xD1, 0x00, 0xED, 0x20, 0xFC, 0xB1, 0x5B, 0x6A, 0xCB, 0xBE, 0x39, 0x4A, 0x4C, 0x58, 0xCF,
   0xD0, 0xEF, 0xAA, 0xFB, 0x43, 0x4D, 0x33, 0x85, 0x45, 0xF9, 0x02, 0x7F, 0x50, 0x3C, 0x9F, 0xA8,
   0x51, 0xA3, 0x40, 0x8F, 0x92, 0x9D, 0x38, 0xF5, 0xBC, 0xB6, 0xDA, 0x21, 0x10, 0xFF, 0xF3, 0xD2,
   0xCD, 0x0C, 0x13, 0xEC, 0x5F, 0x97, 0x44, 0x17, 0xC4, 0xA7, 0x7E, 0x3D, 0x64, 0x5D, 0x19, 0x73,
   0x60, 0x81, 0x4F, 0xDC, 0x22, 0x2A, 0x90, 0x88, 0x46, 0xEE, 0xB8, 0x14, 0xDE, 0x5E, 0x0B, 0xDB,
   0xE0, 0x32, 0x3A, 0x0A, 0x49, 0x06, 0x24, 0x5C, 0xC2, 0xD3, 0xAC, 0x62, 0x91, 0x95, 0xE4, 0x79,
   0xE7, 0xC8, 0x37, 0x6D, 0x8D, 0xD5, 0x4E, 0xA9, 0x6C, 0x56, 0xF4, 0xEA, 0x65, 0x7A, 0xAE, 0x08,
   0xBA, 0x78, 0x25, 0x2E, 0x1C, 0xA6, 0xB4, 0xC6, 0xE8, 0xDD, 0x74, 0x1F, 0x4B, 0xBD, 0x8B, 0x8A,
   0x70, 0x3E, 0xB5, 0x66, 0x48, 0x03, 0xF6, 0x0E, 0x61, 0x35, 0x57, 0xB9, 0x86, 0xC1, 0x1D, 0x9E,
   0xE1, 0xF8, 0x98, 0x11, 0x69, 0xD9, 0x8E, 0x94, 0x9B, 0x1E, 0x87, 0xE9, 0xCE, 0x55, 0x28, 0xDF,
   0x8C, 0xA1, 0x89, 0x0D, 0xBF, 0xE6, 0x42, 0x68, 0x41, 0x99, 0x2D, 0x0F, 0xB0, 0x54, 0xBB, 0x16
};
