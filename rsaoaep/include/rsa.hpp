/*
 * =====================================================================================
 *
 *       Filename:  rsa.hpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  12/20/2014 12:42:44 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  BOSS14420,
 *   Organization:
 *
 * =====================================================================================
 */

#ifndef __RSA_HPP__
#define __RSA_HPP__

#include <utility>
#include <cassert>
#include "bigint.hpp"
#include "primegen.hpp"

namespace rsa {

//#define DUMP(VAR) std::cout << #VAR << " = " << VAR << '\n'
#define DUMP(VAR)


///////////////////////////////////////////////////////////////
////// RSA Public Key
//
template <typename BigInt>
class RSAPublicKey
{
public:
    typedef BigInt Integer;

protected:
    BigInt _n, _e;
    std::size_t _keysize;

public:
    RSAPublicKey() = default;

    template <typename BI1, typename BI2>
    RSAPublicKey(BI1 &&n, BI2 &&e, std::size_t keysize)
        : _n(std::forward<BI1>(n)), _e(std::forward<BI2>(e)),
          _keysize(keysize)
    {}

    BigInt modulus() const { return _n; }

    BigInt exponent() const { return _e; }

    std::size_t keysize() const { return _keysize; }

    BigInt encrypt_int(BigInt const &m) const {
        assert(m < _n);
        DUMP(m);
        BigInt c = bigint::powm(m, _e, _n);
        DUMP(c);
        return c;
    }

    template <typename InputIter, typename OutputIter>
    void encrypt_bytes(InputIter first, InputIter last,
                       OutputIter d_first) const
    {
        assert(std::distance(first, last) < _keysize);
        BigInt m = bigint::bytes_to_int<BigInt>(first, last);
        BigInt c = encrypt_int(m);
        int_to_bytes(m, d_first);
    }
};


///////////////////////////////////////////////////////////////
////// RSA Key
//
template <typename BigInt>
class RSAKey : public RSAPublicKey<BigInt>
{
    BigInt _p, _q;
    BigInt _phi;
    BigInt _d, _dp, _dq, _qinv;

public:
    RSAKey() = default;

    template <typename BI1, typename BI2>
    RSAKey(BI1 &&p, BI2 &&q, std::size_t keysize)
    : RSAPublicKey<BigInt>(p*q, 65537, keysize),
      _p(std::forward<BI1>(p)),
      _q(std::forward<BI2>(q)),
      _phi((p-1)*(q-1)),
      _d(bigint::invmod(this->_e, _phi)),
      _dp(this->_d % (p - 1)),
      _dq(this->_d % (q - 1)),
      _qinv(bigint::invmod(q, p))
    {
        DUMP(this->_n);
        DUMP(this->_e);
        DUMP(this->_keysize);
        DUMP(_p);
        DUMP(_q);
        DUMP(_phi);
        DUMP(_d);
        DUMP(_dp);
        DUMP(_dq);
        DUMP(_qinv);
    }

    RSAPublicKey<BigInt> public_key() const {
        return RSAPublicKey<BigInt>(this->_n, this->_e, this->_keysize);
    }

    BigInt decrypt_int(BigInt const &c) const {
        BigInt m1 = bigint::powm(c, _dp, _p);
        BigInt m2 = bigint::powm(c, _dq, _q);
        BigInt h = (_qinv * (m1 - m2)) % _p;
        while (bigint::sign(h) < 0) h += _p;
        BigInt m = m2 + _q * h;
        return m;
    }

    template <typename InputIter, typename OutputIter>
    void decrypt_bytes(InputIter first, InputIter last,
                        OutputIter d_first) const
    {
        assert(std::distance(first, last) <= this->_keysize / 8);
        BigInt c = bigint::bytes_to_int<BigInt>(first, last);
        BigInt m = decrypt_int(c);
        int_to_bytes(m, d_first);
    }
};

template<typename BigInt>
RSAKey<BigInt> generate_rsakey(std::size_t keysize) {
    assert(keysize % 16 == 0);
    auto p = primegen<BigInt>::random_prime(keysize / 8 / 2);
    auto q = primegen<BigInt>::random_prime(keysize / 8 / 2);
    return RSAKey<BigInt>(p, q, keysize);
}

#undef DUMP

} // namespace rsa
#endif // __RSA_HPP__
