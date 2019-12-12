// ////////////////////////////////////////////////////////
// # Title
// Self powers
//
// # URL
// https://projecteuler.net/problem=48
// http://euler.stephan-brumme.com/48/
//
// # Problem
// The series, `1^1 + 2^2 + 3^3 + ... + 10^10 = 10405071317`.
//
// Find the last ten digits of the series, `1^1 + 2^2 + 3^3 + ... + 1000^1000`.
//
// # Solved by
// Stephan Brumme
// February 2017
//
// # Algorithm
// This problem is pretty easy for languages with support for BigIntegers (such as Java and Python).
// I don't want to use external libraries and thus have to write some code in C++ ...
// welcome to the world of modular arithmetic !
//
// There are two functions:
// - ''mulmod'' is the same as ''(a*b)%modulo''           where ''a*b''           is allowed to exceed 64 bits (''unsigned long long'').
// - ''powmod'' is the same as ''(base^exponent)%modulo'' where ''base^exponent'' is allowed to exceed 64 bits (''unsigned long long'').
//
// ''mulmod'' has two paths:
// - if both factors are small (fit in 32 bits) then there will be no overflow and the original formula ''(a * b) % modulo'' is executed, which is pretty fast
// - else ''a'' and ''b'' are multiplied bitwise (similar to written multiplication), which is much slower
//
// ''powmod'' employs the tricks of https://en.wikipedia.org/wiki/Exponentiation_by_squaring
//
// # Alternative
// GCC supports ''__int128'' which allows to use the fast path of ''mulmod'' all the time.
// See my [toolbox](../toolbox/#mulmod) for more alternatives.

#include <iostream>

// return (a*b) % modulo
// note: NOT USED because mulmod() is faster (see below)
unsigned long long mulmodBitwise(unsigned long long a, unsigned long long b, unsigned long long modulo)
{
  // (a * b) % modulo = (a % modulo) * (b % modulo) % modulo
  a %= modulo;
  b %= modulo;

  // fast path
  if (a <= 0xFFFFFFF && b <= 0xFFFFFFF)
    return (a * b) % modulo;

  // we might encounter overflows (slow path)
  // the number of loops depends on b, therefore try to minimize b
  if (b > a)
    std::swap(a, b);

  // bitwise multiplication
  unsigned long long result = 0;
  while (a > 0 && b > 0)
  {
    // b is odd ? a*b = a + a*(b-1)
    if (b & 1)
    {
      result += a;
      result %= modulo;
      // skip b-- because the bit-shift at the end will remove the lowest bit anyway
    }

    // b is even ? a*b = (2*a)*(b/2)
    a <<= 1;
    a  %= modulo;

    // next bit
    b >>= 1;
  }

  return result;
}

// return (a*b) % modulo
// very similar to mulmodBitWise, but multiple bits are processed at once (instead of just 1 bit per iteration)
unsigned long long mulmod(unsigned long long a, unsigned long long b, unsigned long long modulo)
{
  // (a * b) % modulo = (a % modulo) * (b % modulo) % modulo
  a %= modulo;
  b %= modulo;

  // fast path
  if (a <= 0xFFFFFFF && b <= 0xFFFFFFF)
    return (a * b) % modulo;

  // count leading zero bits of modulo
  unsigned int leadingZeroes = 0;
  unsigned long long m = modulo;
  while ((m & 0x8000000000000000ULL) == 0)
  {
    leadingZeroes++;
    m <<= 1;
  }

  // cover all bits of modulo
  unsigned long long mask = (1 << leadingZeroes) - 1;

  // blockwise multiplication
  unsigned long long result = 0;
  while (a > 0 && b > 0)
  {
    result += (b & mask) * a;
    result %= modulo;

    // next bits
    b >>= leadingZeroes;
    a <<= leadingZeroes;
    a  %= modulo;
  }
  return result;
}

// return (base^exponent) % modulo
unsigned long long powmod(unsigned long long base, unsigned long long exponent, unsigned long long modulo)
{
  unsigned long long result = 1;
  while (exponent > 0)
  {
    // fast exponentation:
    // odd exponent ? a^b = a*a^(b-1)
    if (exponent & 1)
      result = mulmod(result, base, modulo);

    // even exponent ? a^b = (a*a)^(b/2)
    base = mulmod(base, base, modulo);
    exponent >>= 1;
  }
  return result;
}

int main()
{
  // sum from 1^1 to x^x
  unsigned int x;
  std::cin >> x;

  // keep the last 10 digits
  const unsigned long long TenDigits = 10000000000ULL;

  unsigned long long sum = 0;
  // add all parts and don't forget the modulo ...
  for (unsigned int i = 1; i <= x; i++)
    sum += powmod(i, i, TenDigits);

  std::cout << (sum % TenDigits) << std::endl;
  return 0;
}
