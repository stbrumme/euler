// ////////////////////////////////////////////////////////
// # Title
// Primonacci
//
// # URL
// https://projecteuler.net/problem=304
// http://euler.stephan-brumme.com/304/
//
// # Problem
// For any positive integer `n` the function `next\_prime(n)` returns the smallest prime `p` such that `p > n`.
//
// The sequence `a(n)` is defined by:
// `a(1)=next\_prime(10^14)` and `a(n)=next\_prime(a(n-1))` for `n>1`.
//
// The fibonacci sequence `f(n)` is defined by: `f(0)=0`, `f(1)=1` and `f(n)=f(n-1)+f(n-2)` for `n>1`.
//
// The sequence `b(n)` is defined as `f(a(n))`.
//
// Find `sum{b(n)}` for `1<=n<=100 000`. Give your answer mod 1234567891011.
//
// # Solved by
// Stephan Brumme
// August 2017
//
// # Algorithm
// I need two thing for the problem: a fast Fibonacci generator and a prime test for large numbers.
// My toolbox already has a Miller-Rabin test, that means I only have to find a fast Fibonacci generator.
// Last week I solved problem 137 with the matrix form found on https://en.wikipedia.org/wiki/Fibonacci_number
//
// Using those two tools I can easily compute `F(10^14)` and test each consecutive Fibonacci number until I have 100000 primes.
//
// # Alternative
// When I read the Wikipedia a second time I noted a simplification of the matrix form called "fast doubling" on other websites:
// `F_{2n-1} = F^2_n + F^2_{n-1}`
// `F_{2n} = F_n (2 F_{n-1} + F_n)`
//
// My function ''fibonacci'' implements that algorithm. It's about 2 to 3x faster than ''fibonacciMatrix'' (from problem 137).
// Unfortunately, looking for the initial Fibonacci numbers poses a tiny fraction of the overall computation time.
// Therefore it doesn't make a difference - but hopefully my new ''fibonacci'' function can be used in other problems, too.
//
// I played around with my trusted ''powmod'' function, too. It's pretty fast when compiled with GCC but noticeable slower
// with Visual C++ because of its lack of 128-bit integer arithmetic.
// Based on the discussion https://apps.topcoder.com/forums/?module=Thread&threadID=670443&start=0&mc=10 I wrote a second
// ''powmod'' which is about 30% faster on Visual C++ and doesn't need a ''mulmod'' function.
// The old ''powmod'' still remains my fastest implementation of ''powmod'' for GCC.
//
// All these little optimizations caused a little bit of code bloat - that's one of my longest solutions to a Project Euler problem ...
// (actually it became the new number 1 spot on [my list](/performance/)).

#include <iostream>

// ---------- mulmod, powmod and Miller-Rabin test from my toolbox ----------

// return (a*b) % modulo
unsigned long long mulmod(unsigned long long a, unsigned long long b, unsigned long long modulo)
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

#ifndef __GNUC__
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

#else

  // based on GCC's 128 bit implementation
  return ((unsigned __int128)a * b) % modulo;

#endif
}

#ifdef __GNUC__
// return (base^exponent) % modulo => simple implementation
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

#else

// return (base^exponent) % modulo => faster implementation
unsigned long long powmod(unsigned long long base, unsigned long long exponent, unsigned long long modulo)
{
  // based on https://apps.topcoder.com/forums/?module=Thread&threadID=670443&start=0&mc=10
  // fastest generic code (but slower than G++ optimized code)
  unsigned long long result = 1;
  base %= modulo;

  while (exponent > 0)
  {
    // odd exponent ? a^b = a*a^(b-1)
    if (exponent & 1)
    {
      // "unrolled" code of:
      // result = mulmod(result, base, modulo);
      // base   = mulmod(base,   base, modulo);
      auto x = result;
      auto y = base;
      auto b = base;

      result = 0;
      base   = 0;
      while (b > 0)
      {
        if (b & 1)
        {
          result += x; if (result >= modulo) result -= modulo;
          base   += y; if (base   >= modulo) base   -= modulo;
        }

        x += x; if (x >= modulo) x -= modulo;
        y += y; if (y >= modulo) y -= modulo;

        b >>= 1;
      }
    }
    else
    {
      // even exponent ? a^b = (a*a)^(b/2)
      auto y = base;
      auto b = base;

      base = 0;
      while (b > 0)
      {
        if (b & 1)
        {
          base += y; if (base >= modulo) base -= modulo;
        }

        y += y; if (y >= modulo) y -= modulo;
        b >>=1;
      }
    }

    exponent >>=1;
  }

  return result;
}
#endif

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

  // fine-tuning for the problem:
  // fast check of more small primes (up to 100)
  if (p % 19 == 0 || p % 23 == 0 || p % 29 == 0 || p % 31 == 0 || p % 37 == 0 ||
      p % 41 == 0 || p % 43 == 0 || p % 47 == 0 || p % 53 == 0 || p % 59 == 0 ||
      p % 61 == 0 || p % 67 == 0 || p % 71 == 0 || p % 73 == 0 || p % 79 == 0 ||
      p % 83 == 0 || p % 89 == 0 || p % 97 == 0)
    return false;

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

// ---------- compute Fibonacci % modulo ----------

// version 1: matrix algorithm
unsigned long long fibonacciMatrix(unsigned long long n, unsigned long long modulo)
{
  // fast exponentiation: same idea as powmod from my toolbox

  // matrix values from https://en.wikipedia.org/wiki/Fibonacci_number#Matrix_form
  unsigned long long fibo  [2][2]= { { 1, 1 },
                                     { 1, 0 } };
  // initially identity matrix
  unsigned long long result[2][2]= { { 1, 0 },   // { { F(n+1), F(n)   },
                                     { 0, 1 } }; //   { F(n),   F(n-1) } }

  while (n > 0)
  {
    // fast exponentation:
    // odd exponent ? a^n = a*a^(n-1)
    if (n & 1)
    {
      // compute new values, store them in temporaries
      auto t00 = mulmod(result[0][0], fibo[0][0], modulo) + mulmod(result[0][1], fibo[1][0], modulo);
      auto t01 = mulmod(result[0][0], fibo[0][1], modulo) + mulmod(result[0][1], fibo[1][1], modulo);
      auto t10 = mulmod(result[1][0], fibo[0][0], modulo) + mulmod(result[1][1], fibo[1][0], modulo);
      auto t11 = mulmod(result[1][0], fibo[0][1], modulo) + mulmod(result[1][1], fibo[1][1], modulo);

      if (t00 >= modulo) t00 -= modulo;
      if (t01 >= modulo) t01 -= modulo;
      if (t10 >= modulo) t10 -= modulo;
      if (t11 >= modulo) t11 -= modulo;

      // copy back to matrix
      result[0][0] = t00; result[0][1] = t01;
      result[1][0] = t10; result[1][1] = t11;
    }

    // even exponent ? a^n = (a*a)^(n/2)

    // compute new values, store them in temporaries
    auto t00 = mulmod(fibo[0][0], fibo[0][0], modulo) + mulmod(fibo[0][1], fibo[1][0], modulo);
    auto t01 = mulmod(fibo[0][0], fibo[0][1], modulo) + mulmod(fibo[0][1], fibo[1][1], modulo);
    auto t10 = mulmod(fibo[1][0], fibo[0][0], modulo) + mulmod(fibo[1][1], fibo[1][0], modulo);
    auto t11 = mulmod(fibo[1][0], fibo[0][1], modulo) + mulmod(fibo[1][1], fibo[1][1], modulo);

    if (t00 >= modulo) t00 -= modulo;
    if (t01 >= modulo) t01 -= modulo;
    if (t10 >= modulo) t10 -= modulo;
    if (t11 >= modulo) t11 -= modulo;

    // copy back to matrix
    fibo[0][0] = t00; fibo[0][1] = t01;
    fibo[1][0] = t10; fibo[1][1] = t11;

    n >>= 1;
  }

  return result[0][1]; // same as result[1][0]
}

// version 2: fast doubling algorithm, modulo < 2^62
unsigned long long fibonacci(unsigned long long n, unsigned long long modulo)
{
  // extract highest bit
  auto bit = n;
  while (bit & (bit - 1))
    bit &= bit - 1;

  // F(0) and F(1)
  auto a = 0ULL;
  auto b = 1ULL;

  // from highest to lowest bit
  while (bit != 0)
  {
    // F(2n)   = F(n) * (2 * F(n+1) - F(n))
    auto nextA = mulmod(a, 2*b + modulo - a, modulo); // plus modulo to avoid negative results
    // F(2n+1) = F(n)^2 + F(n+1)^2
    b = mulmod(a, a, modulo) + mulmod(b, b, modulo);
    if (b >= modulo)
      b -= modulo;

    a = nextA;

    // odd ?
    if (n & bit)
    {
      // one step further, need F(2n+1) and F(2n+2)
      auto next = a + b;
      if (next >= modulo)
        next -= modulo;

      a = b;
      b = next;
    }

    bit >>= 1;
  }

  return a;
}

int main()
{
  auto n         = 100000000000000ULL;
  auto numPrimes =          100000;
  auto Modulo    =   1234567891011ULL;
  std::cin >> n >> numPrimes >> Modulo;

  // F(n) = F(n-1) + F(n-2)
  // my "seed" values to find F(10^14 + x)
  auto last    = fibonacci(n - 1, Modulo); // replace by fibonacciMatrix for the other algorithm
  auto current = fibonacci(n    , Modulo);
  // actually it's already in result[0][0] of matrix algorithm
  // and the second result of the fast doubling algorithm

  auto sum = 0ULL;
  for (auto i = 1; i <= numPrimes; i++)
  {
    do
    {
      n++;

      // next Fibonacci number
      auto next = (last + current) % Modulo;
      last      = current;
      current   = next;
    }
    while (!isPrime(n));

    // if n is prime then add F(n)
    sum += current;
    // don't overflow
    sum %= Modulo;
  }

  // display result
  std::cout << sum << std::endl;
  return 0;
}
