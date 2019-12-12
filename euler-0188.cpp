// ////////////////////////////////////////////////////////
// # Title
// The hyperexponentiation of a number
//
// # URL
// https://projecteuler.net/problem=188
// http://euler.stephan-brumme.com/188/
//
// # Problem
// The hyperexponentiation or tetration of a number `a` by a positive integer `b`, denoted by `a \upuparrows b` or `{^b}a`, is recursively defined by:
//
// `a \upuparrows 1 = a`,
// `a \upuparrows (k+1) = a(a \upuparrows k)`.
//
// Thus we have e.g. `3 \upuparrows 2 = 3^3 = 27`, hence `3 \upuparrows 3 = 3^27 = 7625597484987` and `3 \upuparrows 4` is roughly `10^{3.6383346400240996 * 10^12}`.
//
// Find the last 8 digits of `1777 \upuparrows 1855`.
//
// # Solved by
// Stephan Brumme
// June 2017
//
// # Algorithm
// ''powmod'' was taken from my [toolbox](../toolbox/).
//
// When I printed intermediate results of ''tetration'' I found that the last digits converge pretty quickly.
// But even without the early-exit optimization the code finishes almost instantly.

#include <iostream>

// return (base^exponent) % modulo for 32-bit values, no need for mulmod
unsigned int powmod(unsigned int base, unsigned int exponent, unsigned int modulo)
{
  unsigned int result = 1;
  while (exponent > 0)
  {
    // fast exponentation:
    // odd exponent ? a^b = a*a^(b-1)
    if (exponent & 1)
      result = (result * (unsigned long long)base) % modulo;

    // even exponent ? a^b = (a*a)^(b/2)
    base = (base * (unsigned long long)base) % modulo;
    exponent >>= 1;
  }
  return result;
}

// compute result
unsigned int tetration(unsigned int a, unsigned int b, unsigned int modulo)
{
  unsigned int last   = 0;
  unsigned int result = 1;
  while (b--)
  {
    result = powmod(a, result, modulo);

    // converges pretty fast, abort early
    if (last == result)
      break;
    last = result;
  }
  return result;
}

int main()
{
  unsigned int a = 1777;
  unsigned int b = 1855;
  std::cin >> a >> b;

  unsigned int modulo = 100000000;

  // abort if gcd(a, modulo) != 1
  if (a % 10 == 0)
    return 1;

  std::cout << tetration(a, b, modulo) << std::endl;
  return 0;
}
