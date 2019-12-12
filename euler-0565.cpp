// ////////////////////////////////////////////////////////
// # Title
// Divisibility of sum of divisors
//
// # URL
// https://projecteuler.net/problem=565
// http://euler.stephan-brumme.com/565/
//
// # Problem
// Let `sigma(n)` be the sum of the divisors of `n`.
// E.g. the divisors of 4 are 1, 2 and 4, so `sigma(4)=7`.
//
// The numbers `n` not exceeding 20 such that 7 divides `sigma(n)` are: 4,12,13 and 20, the sum of these numbers being 49.
//
// Let `S(n,d)` be the sum of the numbers `i` not exceeding `n` such that `d` divides `sigma(i)`.
// So `S(20,7)=49`.
//
// You are given: `S(10^6,2017)=150850429` and `S(10^9,2017)=249652238344557`
//
// Find `S(10^11,2017)`.
//
// # Solved by
// Stephan Brumme
// December 2017
//
// # Algorithm
// The Wikipedia page about the Divisor Function has a few useful formulas (see https://en.wikipedia.org/wiki/Divisor_function ):
// (1) `\sigma_1(p) = p + 1` if `p` is prime
//
// (2) `\sigma_1(p^k) = dfrac{p^{k+1} - 1}{p - 1}` if `p` is prime
//
// (3) `\sigma_1(m * n) = \sigma_1(m) * \sigma_1(n)` if `m` and `n` are coprime
//
// The most crucial observation is that if `\sigma_1(m)` is divisible by 2017, then according to (3) almost every multiple is, too
// (the exception is when `m` and `n` are not coprime, for example for `m = n`).
//
// My function ''search()'' starts with equation (2) (and `k >= 2`) because there are only few such numbers.
// I check for every prime whether `\sigma_1(p^k) = dfrac{p^{k+1} - 1}{p - 1}` is a multiple of 2017.
// If yes, then that number including all its multiples (except every `p`-th multiple) has a divisor sum that is divisible by 2017.
// Since `p^2 <= 10^11` less than a million number have to be checked which is done in less than a second.
//
// Much more computation time is spent on equation (1):
// every prime `p` where `p+1` is a multiple of 2017 and its multiples have a matching divisor sum.
// Instead of finding all primes below `10^11` I check every number `2017k - 1` whether it is prime.
// The Miller-Rabin test from my [toolbox](/toolbox/) is pretty fast but I had to include a GCC optimization to stay below one minute execution time.
//
// A substantial part of my code is devoted to detecting duplicates: a few number are found multiple times.
// For example 12101 is the smallest prime where `\sigma_1` is divisible by 2017. The next prime is 24203.
// Their product 12101 * 24203 = 292880503 will be found while processing all multiples of 12101 and then again while processing all multiples of 24203.
// The container ''found'' is a simple ''std::vector'' because it has pretty much zero data structure overhead (unlike ''std::set'').
//
// Duplicates (or a "collision") is impossible for the first 12101-1=12100 multiples of a matching prime (since 12101 is the smallest).
// To keep my code more generic I simplified that idea and only assume that a collision is impossible for the first 2017-1=2016 multiple.
// If the current prime `p` is larger than `sqrt{10^11}` then a collision can only occur if the multiple was seen before because it must be smaller than `p.
// Therefore I sort the list of primes found so far and perform a lookup with binary search.
// All these optimizations have only one goal: reduce the size of ''found'' until it drops below the 256 MByte limit.
//
// When all primes are processed then ''found'' is sorted again to eliminate all duplicates with ''std::unique''.
//
// # Note
// I had the solution pretty fast but my code needed about 6 minutes and almost 2 GByte RAM.
// Most of the time was spent optimizing the code (e.g. refactor from ''std::set'' to ''std::vector'').
// And while optimizing, I "broke" the algorithm quite a few times ...
//
// Right now my solution has still high memory consumption and execution time.
// It's by far the highest ranking [expensive solution](/performance/#slowandbig) which doesn't exceed the memory or CPU limits.

#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

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

#ifdef __GNUC__
  // based on GCC's 128 bit implementation
  return ((unsigned __int128)a * b) % modulo;
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

  if (p < 17 * 19) // we filtered all composite numbers < 17*19, all others below 17*19 must be prime
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

// ---------- problem-specific code ----------

// much faster ... but still takes a few seconds
unsigned long long search(unsigned long long limit, unsigned int multiple)
{
  std::vector<unsigned long long> found;
  // avoid re-allocations for limit = 10^11
  if (limit == 100000000000ULL)
    found.reserve(26240000);

  // find primes where sigma(prime^2) is divisible by 2017 (same for sigma(prime^3), ...)
  for (unsigned long long p = 2; p*p <= limit; p++)
  {
    if (!isPrime(p))
      continue;

    // at least p^2
    auto power = p * p;
    while (power <= limit)
    {
      // sigma = (p^q - 1) / (p - 1)
      auto sigma = (power*p - 1) / (p - 1);
      if (sigma % multiple == 0)
        // including all multiples
        for (auto i = 1; i*power <= limit; i++)
          if (i % p != 0) // avoid the case where i is a multiple of p because then they aren't coprime anymore
            found.push_back(i*power);

      // next iteration will exceed limit ?
      if (limit / power < p)
        break;

      // keep going ...
      power *= p;
    }
  }

  // sum of all matches
  unsigned long long result = 0;

  // switch to "optimized" mode if p^2 > limit
  size_t sortedSize = 0;

  // sigma(prime) = prime + 1
  // => check all numbers 2017k - 1 whether they are prime
  for (unsigned long long p = multiple - 1; p <= limit; p += multiple)
  {
    // only primes ...
    if (!isPrime(p))
      continue;

    // "optimized" mode:
    // collisions can only occur when multiplying p by a smaller prime which must be part of the "found" container
    if (sortedSize == 0 && p*p > limit)
    {
      // sort all solutions so that I can search for elements with std::binary_search
      std::sort(found.begin(), found.end());
      sortedSize = found.size();
    }

    // sigma(p) is a multiple of 2017 and that's also true for all its multiples
    for (unsigned long long i = 1; i*p <= limit; i++)
    {
      // avoid the case where i is a multiple of p because then they aren't coprime anymore
      if (i % p == 0)
        continue;

      // another solution (which might be a duplicate, though !)
      auto current = i * p;

      // if i is small then a collision is impossible
      if (i < multiple - 1)
      {
        result += current;
        continue;
      }

      // I can detect collisions immediately if p > sqrt(limit) because then p > i
      if (sortedSize > 0)
      {
        if (!std::binary_search(found.begin(), found.begin() + sortedSize, current))
          result += current;
      }
      else
        // potential collision
        found.push_back(current);
    }
  }

  // exclude duplicates
  std::sort(found.begin(), found.end());
  auto duplicates = std::unique(found.begin(), found.end());
  // sum of all found numbers (but count duplicates only once)
  for (auto i = found.begin(); i != duplicates; i++)
    result += *i;

  return result;
}

int main()
{
  unsigned long long limit = 100000000000ULL; // 10^11
  unsigned int    multiple = 2017;
  std::cin  >> limit >> multiple;
  std::cout << search(limit, multiple) << std::endl;
  return 0;
}

