// ////////////////////////////////////////////////////////
// # Title
// Composites with prime repunit property
//
// # URL
// https://projecteuler.net/problem=130
// http://euler.stephan-brumme.com/130/
//
// # Problem
// A number consisting entirely of ones is called a repunit. We shall define `R(k)` to be a repunit of length `k`; for example, `R(6) = 111111`.
//
// Given that n is a positive integer and `GCD(n, 10) = 1`, it can be shown that there always exists a value, `k`,
// for which `R(k)` is divisible by `n`, and let `A(n)` be the least such value of `k`; for example, `A(7) = 6` and `A(41) = 5`.
//
// You are given that for all primes, `p > 5`, that `p - 1` is divisible by `A(p)`. For example, when `p = 41`, `A(41) = 5`, and 40 is divisible by 5.
//
// However, there are rare composite values for which this is also true; the first five examples being 91, 259, 451, 481, and 703.
//
// Find the sum of the first twenty-five composite values of `n` for which `GCD(n, 10) = 1` and `n - 1` is divisible by `A(n)`.
//
// # Solved by
// Stephan Brumme
// July 2017
//
// # Algorithm
// Copying ''getMinK'' from problem 129 works fine and shows the correct result after a few milliseconds.
//
// However, due to the modified Hackerrank problem I had to change my algorithm from the ground up:
// large numbers require a primality test that can handle number `approx 10^12` ==> Miller-Rabin test from my [toolbox](/toolbox/)
//
// And it took me some time to realize that there is no need to find the smallest `k`:
// if `p-1` is a multiple of `A(p)` then `R(A(p))` can be divided by `p-1`.
// When I look at `R(2 * A(p))` then it obviously has twice as many digits as `R(A(p))`.
// But `R(2 * A(p))` is a multiple of `R(A(p))`: `R(2 * A(p)) = (10^p + 1) * R(A(p))`.
// Let's assume that `A(p) = 2` ==> `R(2) = 11` and `R(2 * 2) = R(4) = 1111 = (10^2 + 1) * 11 = 101 * 11`
//
// Therefore I simply check whether `R(p-1)` (instead of `R(A(p))`) is a multiple of `p`.
// And there is a pretty fast test for this:
// `R(p-1) = 111...111 = dfrac{10^{p-1}}{9} - 1`
// `1 = dfrac{10^{p-1}}{9 * R(p - 1)}`
//
// Using ''powmod'', which I need anyway for the Miller-Rabin test, I can find the residue of ''powmod(10, p-1, 9*p)'' extremely fast
// ==> if it's 1, then I have a match.
//
// # Hackerrank
// Aside from the larger input range, I have to print all matches instead of the sum of the first 25 matches.
// But my code is still not good enough to solve all their test cases - I only get a 48% score.

#include <iostream>
#include <vector>

#define ORIGINAL

// ---------- Miller-Rabin test from my toolbox ----------

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
      x = mulmod(x, x, p);
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

// ---------- getMinK from problem 129 ----------

// return minimum k where R(k) is divisible by x
unsigned long long getMinK(unsigned long long x)
{
  // same as gcd(x, 10) = 1
  if (x % 2 == 0 || x % 5 == 0)
    return 0;

  // "number of ones"
  unsigned long long result  = 1;
  // current repunit mod divisor
  unsigned long long repunit = 1;
  // no remainder ? that repunit can be divided by divisor
  while (repunit != 0)
  {
    // next repunit
    repunit *= 10;
    repunit++;
    // keep it mod divisor
    repunit %= x;

    result++;
  }

  return result;
}

int main()
{
  unsigned long long from = 2;
  unsigned long long to   = 15000;

#ifdef ORIGINAL
  unsigned int maxFound = 25;
  unsigned int numFound = 0;
  unsigned int sum = 0;

#else
  std::cin >> from >> to;
#endif

  // for all even numbers gcd(i, 10) != 1 (it's >= 2)
  if (from % 2 == 0)
    from++;
  // 91 is the first match
  if (from < 91)
    from = 91;

  for (unsigned int p = from; p <= to; p += 2)
  {
    // reject prime numbers
    if (isPrime(p))
      continue;

    // my old code based on problem 129:
    // find minimum k (can be zero if gcd(p, 10) == 1)
    //auto k = getMinK(p);
    //if (k == 0)
      //continue;
    //if ((p - 1) % k == 0)

    // ==> replaced by much faster idea:
    // don't look for the smallest k
    // just test if R(i-1) is a multiple of i
    if (powmod(10, p - 1, 9 * p) == 1)
    {
#ifdef ORIGINAL
      sum += p;
      numFound++;
      if (numFound == maxFound)
        break;
#else
      std::cout << p << std::endl;
#endif
    }
  }

#ifdef ORIGINAL
  std::cout << sum << std::endl;
#endif

  return 0;
}
