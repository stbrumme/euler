// ////////////////////////////////////////////////////////
// # Title
// Spiral primes
//
// # URL
// https://projecteuler.net/problem=58
// http://euler.stephan-brumme.com/58/
//
// # Problem
// Starting with 1 and spiralling anticlockwise in the following way, a square spiral with side length 7 is formed.
//
// ''__37__ 36 35 34 33 32 31''
// ''38 17 16 15 14 13 30''
// ''39 18  5  4  3 12 29''
// ''40 19  6  1  2 11 28''
// ''41 20  7  8  9 10 27''
// ''42 21 22 23 24 25 26''
// ''43 44 45 46 47 48 49''
//
// It is interesting to note that the odd squares lie along the bottom right diagonal, but what is more interesting is that
// 8 out of the 13 numbers lying along both diagonals are prime; that is, a ratio of `frac{8}{13} approx 62%`.
//
// If one complete new layer is wrapped around the spiral above, a square spiral with side length 9 will be formed.
// If this process is continued, what is the side length of the square spiral for which the ratio of primes along both diagonals first falls below 10%?
//
// # Solved by
// Stephan Brumme
// February 2017
//
// # Algorithm
// My solution consists almost entirely of "old code":
// - the Miller-Rabin primality test from problem 50 ...
// - which in turn requires modular arithmetic from problem 48
//
// That code can be found in my [toolbox](../toolbox/), too.
//
// # Hackerrank
// My portable ''mulmod'' was a little bit too slow for Hackerrank, therefore I added the "GCC"-Hack:
// the GCC-extension ''__int128'' simplifies ''mulmod'' to a one-line function (which is much faster, too).

#include <iostream>

// return (a*b) % modulo
unsigned long long mulmod(unsigned long long a, unsigned long long b, unsigned long long modulo)
{
#ifdef __GNUC__
  // use GCC's optimized 128 bit code
  return ((unsigned __int128)a * b) % modulo;
#endif

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
      if (result >= modulo)
        result -= modulo;
      // skip b-- because the bit-shift at the end will remove the lowest bit anyway
    }

    // b is even ? a*b = (2*a)*(b/2)
    a <<= 1;
    if (a >= modulo)
      a -= modulo;

    // next bit
    b >>= 1;
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

// Miller-Rabin-test
bool isPrime(unsigned long long p)
{
  // IMPORTANT: requires mulmod(a, b, modulo) and powmod(base, exponent, modulo)

  // some code from             https://ronzii.wordpress.com/2012/03/04/miller-rabin-primality-test/
  // with optimizations from    http://ceur-ws.org/Vol-1326/020-Forisek.pdf
  // good bases can be found at http://miller-rabin.appspot.com/

  // trivial cases
  const unsigned int bitmaskPrimes2to31 = (1 <<  2) | (1 <<  3) | (1 <<  5) | (1 <<  7) |
                                          (1 << 11) | (1 << 13) | (1 << 17) | (1 << 19) |
                                          (1 << 23) | (1 << 29); // = 0x208A28Ac
  if (p < 31)
    return (bitmaskPrimes2to31 & (1 << p)) != 0;

  if (p %  2 == 0 || p %  3 == 0 || p %  5 == 0 || p % 7 == 0 || // divisible by a small prime
      p % 11 == 0 || p % 13 == 0 || p % 17 == 0)
    return false;

  if (p < 17*19) // we filtered all composite numbers < 17*19, all others below 17*19 must be prime
    return true;

  // test p against those numbers ("witnesses")
  // good bases can be found at http://miller-rabin.appspot.com/
  const unsigned int STOP = 0;
  const unsigned int TestAgainst1[] = { 377687, STOP };
  const unsigned int TestAgainst2[] = { 31, 73, STOP };
  const unsigned int TestAgainst3[] = { 2, 7, 61, STOP };
  // first three sequences are good up to 2^32
  const unsigned int TestAgainst4[] = { 2, 13, 23, 1662803, STOP };
  const unsigned int TestAgainst7[] = { 2, 325, 9375, 28178, 450775, 9780504, 1795265022, STOP };

  // good up to 2^64
  const unsigned int* testAgainst = TestAgainst7;
  // use less tests if feasible
  if (p < 5329)
    testAgainst = TestAgainst1;
  else if (p < 9080191)
    testAgainst = TestAgainst2;
  else if (p < 4759123141ULL)
    testAgainst = TestAgainst3;
  else if (p < 1122004669633ULL)
    testAgainst = TestAgainst4;

  // find p - 1 = d * 2^j
  auto d = p - 1;
  d >>= 1;
  unsigned int shift = 0;
  while ((d & 1) == 0)
  {
    shift++;
    d >>= 1;
  }

  // test p against all bases
  do
  {
    auto x = powmod(*testAgainst++, d, p);
    // is test^d % p == 1 or -1 ?
    if (x == 1 || x == p - 1)
      continue;

    // now either prime or a strong pseudo-prime
    // check test^(d*2^r) for 0 <= r < shift
    bool maybePrime = false;
    for (unsigned int r = 0; r < shift; r++)
    {
      // x = x^2 % p
      // (initial x was test^d)
      x = powmod(x, 2, p);
      // x % p == 1 => not prime
      if (x == 1)
        return false;

      // x % p == -1 => prime or an even stronger pseudo-prime
      if (x == p - 1)
      {
        // next iteration
        maybePrime = true;
        break;
      }
    }

    // not prime
    if (!maybePrime)
      return false;
  } while (*testAgainst != STOP);

  // prime
  return true;
}

int main()
{
  unsigned int percentage = 10;
  std::cin >> percentage;

  // the lower right diagonal contains only squares
  unsigned long long numPrimes  = 0;
  unsigned long long sideLength = 1;
  unsigned long long diagonals  = 1;
  do
  {
    sideLength += 2;
    diagonals  += 4;

    unsigned long long lowerRight = sideLength * sideLength;
    unsigned long long lowerLeft  = lowerRight - (sideLength - 1);
    unsigned long long upperLeft  = lowerLeft  - (sideLength - 1);
    unsigned long long upperRight = upperLeft  - (sideLength - 1);

    // no need to test lowerRight since it's a square and never prime
    if (isPrime(lowerLeft)  != 0)
      numPrimes++;
    if (isPrime(upperLeft)  != 0)
      numPrimes++;
    if (isPrime(upperRight) != 0)
      numPrimes++;
  } while (numPrimes * 100 / diagonals >= percentage);

  std::cout << sideLength << std::endl;
  return 0;
}
