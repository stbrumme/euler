// ////////////////////////////////////////////////////////
// # Title
// Large non-Mersenne prime
//
// # URL
// https://projecteuler.net/problem=97
// http://euler.stephan-brumme.com/97/
//
// # Problem
// The first known prime found to exceed one million digits was discovered in 1999,
// and is a Mersenne prime of the form `2^6972593 - 1`; it contains exactly 2,098,960 digits.
// Subsequently other Mersenne primes, of the form `2^p - 1`, have been found which contain more digits.
//
// However, in 2004 there was found a massive non-Mersenne prime which contains 2,357,207 digits: `28433 * 2^7830457 + 1`.
//
// Find the last ten digits of this prime number.
//
// # Solved by
// Stephan Brumme
// March 2017
//
// # Algorithm
// I used ''powmod'' in problem 48 to compute `a^b mod c` (see there for an explanation of the way ''powmod'' works).
// Unfortunately we have to find the 10 last digits, which is one digit too much for 32/64 bit multiplications.
// GCC's support for 64/128 arithmetic solves this problem easily.
//
// # Alternative
// I could have used my old ''powmod'' code which relies on ''mulmod'' but that code is too slow for the Hackerrank version of this problem.
// On the contrary, that code from problem 48 is more portable.

#include <iostream>
#include <iomanip>

// GCC only !!!
typedef unsigned __int128 BigNum;

//#define ORIGINAL
#ifdef ORIGINAL
const unsigned int Digits = 10;
const BigNum       Modulo = 10000000000ULL;
#else
const unsigned int Digits = 12;
const BigNum       Modulo = 1000000000000ULL;
#endif

// compute the n-th power of a big number (n >= 0)
BigNum powmod(BigNum base, unsigned int exponent, BigNum modulo)
{
  BigNum result = 1;
  while (exponent > 0)
  {
    // fast exponentiation
    if (exponent & 1)
      result = (result * base) % modulo;

    base = (base * base) % modulo;
    exponent >>= 1;
  }
  return result;
}

int main()
{
  unsigned long long sum = 0;

  unsigned int tests = 1;
  std::cin >> tests;
  while (tests--)
  {
    // read a * b^c + d
    unsigned long long factor, base, exponent, add;
    std::cin >> factor >> base >> exponent >> add;

    // compute result
    unsigned long long result = (powmod(base, exponent, Modulo) * factor + add) % Modulo;

    // modulo all the way ... we need only the last 10 (or 12) digits
    sum += result;
    sum %= Modulo;
  }

  // print with leading zeros
  std::cout << std::setfill('0') << std::setw(Digits) << sum;
  return 0;
}
