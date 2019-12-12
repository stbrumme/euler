// ////////////////////////////////////////////////////////
// # Title
// Investigating the primality of numbers of the form 2n^2-1
//
// # URL
// https://projecteuler.net/problem=216
// http://euler.stephan-brumme.com/216/
//
// # Problem
// Consider numbers `t(n)` of the form `t(n) = 2n^2-1` with `n > 1`.
// The first such numbers are 7, 17, 31, 49, 71, 97, 127 and 161.
// It turns out that only `49 = 7 * 7` and `161 = 7 * 23` are not prime.
// For `n <= 10000` there are 2202 numbers `t(n)` that are prime.
//
// How many numbers `t(n)` are prime for `n <= 50,000,000` ?
//
// # Solved by
// Stephan Brumme
// November 2017
//
// # Algorithm
// The Miller-Rabin primality test from my toolbox can easily solve this problem.
// The only drawback: it takes xyz minutes.
//
// I still can't solve this problem in under a minute but at least reduced the runtime considerably by observing that
// if `t(n) = 2n^2 - 1` is a multiple of number `k` then `t(n+k) = 2(n + k)^2 - 1` is a multiple of `k` too:
// `t(n + k) - t(n)`
// `= (2(n + k)^2 - 1) - (2n^2 - 1)`
// `=  2(n + k)^2      -  2n^2`
// `=  2(n^2 + 2nk + k^2) - 2n^2`
// `=   2n^2 + 4nk + 2k^2 - 2n^2`
// `=          4nk + 2k^2       `
// `= k * (4n + 2k)`
//
// In fact, if `t(n)` is a multiple of such `k` then not only `t(n + k)` but also `t(n + 2k)` and `t(n + 3k)` and so on are multiples of `k`.
// And that means they can't be prime - reducing the number of Miller-Rabin tests to about one fifth.
//
// I don't have a fast factorization algorithm in my toolbox yet, so I perform simple trial division to find prime factors.
// After trying several values I found that it doesn't pay off to check for prime factors larger than one million (see ''MaxSievePrime'').
// Even more, I hardly found any candidates above two millions that are divisible by a prime which wasn't already a factor of a smaller candidates (see ''FilterThreshold'').
// Both constants were heuristically determined by lots of trial'n'error.
//
// # Note
// OpenMP gives a nice speed-up but I still need about 6 minutes to find the correct result (see ''#define PARALLEL'').
// The single-thread version finishes after about 33 minutes.
// By the way: if I would use only my Miller-Rabin test (without the optimizations mentioned above), the program finishes after 55 minutes.
//
// Reading the forums, the vast majority of solvers seem to have a simple loop invoking the prime test available in Java, Mathematica, etc.
// They neither wrote the prime test nor looked for optimizations.
// In my opinion, this is a quite hard problem if you really want to stick to the "one-minute rule".
//
// Looking at the high number of solvers and the low difficulty rating I expected that I missed something big - but actually only a small number of people
// discovered/knew the most appropriate way to solve this problem, the Tonelli-Shanks algorithm.
//
// # Alternative
// The Tonelli-Shanks algorithm (which I wasn't aware of) is much faster.
// I probably should translate [Wikipedia's pseudo-code](https://en.wikipedia.org/wiki/Tonelli%E2%80%93Shanks_algorithm) to C++ and add it to my toolbox.

#include <iostream>
#include <vector>
#include <algorithm>

// ---------- copied the Miller-Rabin primality test from my toolbox ----------

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

// ---------- problem-specific code ----------

int main()
{
  unsigned int limit = 50000000;
  std::cin >> limit;

  // count all prime 2n^2-1
  unsigned int count = 0;

  // if candidate[n] is false, then n can't be a prime, else there is a possibility
  std::vector<bool> candidate(limit + 1, true);

  // generate a few primes, use slower Miller-Rabin algorithm for convenience
  const auto MaxSievePrime = std::max<unsigned int>(limit / 50, 10000);
  std::vector<unsigned int> smallPrimes;
  for (unsigned int p = 3; p <= MaxSievePrime; p++)
    if (isPrime(p))
      smallPrimes.push_back(p);
  // almost all numbers above this threshold have bigger factors (or are primes)
  const auto FilterThreshold = 2 * MaxSievePrime;

  // accelerate with OpenMP
#define PARALLEL
#ifdef  PARALLEL
  unsigned int numCores = 0; // 0 => run on all cores, 1,2,3,... => restrict to 1,2,3,... cores
#pragma omp parallel for reduction(+:count) num_threads(numCores) schedule(dynamic, 10000)
#endif
  for (unsigned long long n = 2; n <= limit; n++)
  {
    // already filtered ?
    if (!candidate[n])
      continue;

    // calculate 2n^2-1
    auto p = 2 * n * n - 1;
    // run primality test
    if (isPrime(p))
    {
      count++;
      continue;
    }

    // p failed the primality test, figure out which (small) prime is a factor of p
    // I didn't observe many "new" small prime factors above a certain threshold
    if (n < FilterThreshold)
      for (auto s : smallPrimes)
        if (p > s && p % s == 0) // trial division
          for (auto i = n; i <= limit; i += s) // remove all multiples
            candidate[i] = false;
  }

  // display result
  std::cout << count << std::endl;
  return 0;
}
