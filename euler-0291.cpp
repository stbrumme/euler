// ////////////////////////////////////////////////////////
// # Title
// Panaitopol Primes
//
// # URL
// https://projecteuler.net/problem=291
// http://euler.stephan-brumme.com/291/
//
// # Problem
// A prime number p is called a Panaitopol prime if `p=dfrac{x^4-y^4}{x^3+y^3}` for some positive integers `x` and `y`.
//
// Find how many Panaitopol primes are less than `5 * 10^15`.
//
// # Solved by
// Stephan Brumme
// July 2017
//
// # Algorithm
// It's easy to make a brute force search for all Panaitopol prime below 1000.
// After about a minute my program displayed 5,13,41,61,113,181,313,421,613,761 (see ''isPanaitopolPrime'' and ''#define BRUTE_FORCE'').
// I entered this sequence into my favorite internet search engine and found immediately https://oeis.org/A027862
//
// They claimed that each Panaitopol prime can be represented as `n^2 + (n+1)^2`.
// All I did is copying my Miller-Rabin primality check from my [toolbox](/toolbox/) and testing each number
// ''current = n*n + (n+1)*(n+1)'' whether it's prime.
//
// It takes about 2 minutes on my machine which exceeds the inofficial time limit of Project Euler.
// I saw a few different approaches which are much faster but I don't fully understand the mathematics
// (and honestly I can't fully follow the reasoning why each Panaitopol prime is part of `n^2 + (n+1)^2`).
//
// # Note
// ''mulmod'' was tuned for the 64-bit GCC compiler (on average about 4000 CPU cycles per test ==> 1 million tests per second / single-threaded).
// Visual C++ is much slower.
//
// In September 2017 I added OpenMP support. My 8-core computer finds the correct result in less than 20 seconds now.
// However, the single-core performance remains unchanged and I still label my solution as "inefficient".

#include <iostream>
#include <iomanip>
#include <cmath>

// ---------- my initial brute-force code ----------

// brute-force search
// only good up for panaitopol primes below 1000
bool isPanaitopolPrime(unsigned int p)
{
  // simple prime check
  if (p % 2 == 0)
    return p == 2;

  for (unsigned int i = 3; i*i <= p; i += 2)
    if (p % i == 0)
      return false;

  for (unsigned long long x = 1; x < 20*p; x++)
    for (unsigned long long y = 1; y < x; y++)
    {
      auto num = x*x*x*x - y*y*y*y;
      auto den = x*x*x   + y*y*y;

      if (num <= den)
        continue;
      if (num % den != 0)
        continue;

      if (num / den != p)
        continue;

      std::cout << "prime=" << p << " x=" << x << " y=" << y << std::endl;
      return true;
    }

  return false;
}

// ---------- Miller-Rabin prime test ----------

// return (a*b) % modulo
unsigned long long mulmod(unsigned long long a, unsigned long long b, unsigned long long modulo)
{
  // (a * b) % modulo = (a % modulo) * (b % modulo) % modulo
  a %= modulo;
  b %= modulo;

  // fast path
  if (a <= 0xFFFFFFF && b <= 0xFFFFFFF)
    return (a * b) % modulo;

#ifdef __GNUC__
#ifdef __x86_64__
  // use GCC inline assembler
  unsigned long long asmResult;
  __asm__
  (
    "mulq %2\n" // result in rdx:rax
    "divq %3"   // quotient in rax, remainder in rdx
    : "=&d" (asmResult), "+%a" (a)
    : "rm" (b), "rm" (modulo)
    : "cc"      // clear conditions
  );
  return asmResult;

#else

  // based on GCC's 128 bit implementation
  return ((unsigned __int128)a * b) % modulo;
#endif
#endif

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

int main()
{
  // result
  unsigned long long count = 0;

//#define BRUTE_FORCE
#ifdef BRUTE_FORCE
  // super-slow brute-force approach to find the first numbers
  for (unsigned int i = 1; i < 1000; i++)
    if (isPanaitopolPrime(i))
      count++;
#else

  // 5*10^^15
  auto limit = 5000000000000000ULL;
  std::cin >> limit;

  // prime can be represented as n^2 + (n+1)^2
  // which is roughly 2 * (n+1)^2
  unsigned long long last = sqrt(limit / 2);
//#define PARALLEL
#ifdef  PARALLEL
#pragma omp parallel for reduction(+:count) schedule(dynamic)
#endif
  for (unsigned long long n = 1; n <= last; n++) // abort condition is inside the loop
  {
    // find next candidate
    auto current = n*n + (n+1)*(n+1);
    if (current <= limit && isPrime(current))
      count++;
  }
#endif

  // display result
  std::cout << count << std::endl;
  return 0;
}
