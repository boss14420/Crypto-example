/*
 * =====================================================================================
 *
 *       Filename:  aestest.cc
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  12/19/2014 08:18:34 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (),
 *   Organization:
 *
 * =====================================================================================
 */


#include <cstdio>
#include "aes.hpp"
#include <vector>

int main(int argc, char *argv[]) {
    std::vector<unsigned char> plain(32), cipher(48), plain2(48);
    AES<128>::Key key {{ 0x0f, 0x15, 0x71, 0xc9, 0x47, 0xd9, 0xe8, 0x59,
                         0x0c, 0xb7, 0xad, 0xd6, 0xaf, 0x7f, 0x67, 0x98 }};

//    AES<128> aes(key);
//
//    aes.encrypt_block(plain, cipher);
//
//    for (auto i = 0; i != 16; ++i)
//        std::printf("%02x ", cipher[i]);
//    std::putchar('\n');

    using std::begin;
    using std::end;

//    AES_CTRMode<128> aes(key, nonce);
//    auto ci = aes.encrypt_append(&plain[0], &plain[18], begin(cipher));
//    ci = aes.encrypt_append(&plain[18], &plain[29], ci);
//    ci = aes.encrypt_finish(&plain[29], &plain[29], ci);
//
//    for (auto ii = begin(cipher); ii != ci; ++ii)
//        std::printf("%02x ", *ii);
//    std::putchar('\n');
//
//    AES_CTRMode<128> aes2(key, nonce);
//    auto di = aes2.decrypt_finish(cipher.begin(), ci, &plain2[0]);
//
//    for (auto ii = &plain2[0]; ii != di; ++ii)
//        std::printf("%02x ", *ii);
//    std::putchar('\n');

    char mode = 'e';
    if (argc > 1) mode = argv[1][0];
    std::vector<unsigned char> input(16 * 1000), output(16 * 1000);
    if (mode != 'd') {
        // encrypt nonce
        std::uint64_t nonce = 0x123456789abcdef0;
        std::fwrite((char*)&nonce, 1, 8, stdout);

        // encrypt input data
        AES_CTRMode<128> aesctr(key, nonce);
        unsigned char *oi = output.data();
        while (!std::feof(stdin)) {
            auto nr = std::fread(input.data(), 1, input.size(), stdin);
            oi = aesctr.encrypt_append(input.data(), input.data() + nr, output.data());
            std::fwrite(output.data(), 1, (oi - output.data()), stdout);
        }
        oi = aesctr.encrypt_finish(input.data(), input.data(), output.data());
        std::fwrite(output.data(), 1, (oi - output.data()), stdout);
    } else {
        // decrypt nonce
        std::uint64_t nonce;
        std::fread((char*)&nonce, 1, 8, stdin);

        // decrypt input data
        AES_CTRMode<128> aesctr(key, nonce);
        unsigned char *oi = output.data();
        while (!std::feof(stdin)) {
            auto nr = std::fread(input.data(), 1, input.size(), stdin);
            if (nr < input.size() || std::feof(stdin))
                oi = aesctr.decrypt_finish(input.data(), input.data() + nr, output.data());
            else
                oi = aesctr.decrypt_append(input.data(), input.data() + nr, output.data());
            std::fwrite(output.data(), 1, (oi - output.data()), stdout);
        }
//        oi2 = aesctr.decrypt_finish(input.data(), input.data(), oi);
    }

    return 0;
}
