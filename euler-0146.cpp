// ////////////////////////////////////////////////////////
// # Title
// Numbers for which no three consecutive digits have a sum greater than a given value
//
// # URL
// https://projecteuler.net/problem=164
// http://euler.stephan-brumme.com/164/
//
// # Problem
// The smallest positive integer `n` for which the numbers `n^2+1`, `n^2+3`, `n^2+7`, `n^2+9`, `n^2+13`, and `n^2+27` are consecutive primes is 10.
// The sum of all such integers `n` below one-million is 1242490.
//
// What is the sum of all such integers `n` below 150 million?
//
// # Solved by
// Stephan Brumme
// June 2017
//
// # Algorithm
// A primitive approach is:
// ''if ( isPrime(n*n +  1) &&''
// ''     isPrime(n*n +  3) &&''
// ''    !isPrime(n*n +  5) &&''
// ''     isPrime(n*n +  7) &&''
// ''     isPrime(n*n +  9) &&''
// ''    !isPrime(n*n + 11) &&''
// ''     isPrime(n*n + 13) &&''
// ''    !isPrime(n*n + 15) &&''
// ''    !isPrime(n*n + 17) &&''
// ''    !isPrime(n*n + 19) &&''
// ''    !isPrime(n*n + 21) &&''
// ''    !isPrime(n*n + 23) &&''
// ''    !isPrime(n*n + 25) &&''
// ''     isPrime(n*n + 27))''
//
// My Miller-Rabin primality tests easily finds the correct answer - but is much too slow (it takes several minutes).
// That primality test was more or less copied from my [toolbox](../toolbox/). I only added an optional parameter ''fastCheckAgainstSmallPrimes'' (see more on that later).
//
// Most of the code in ''main'' tries is about speeding up the whole algorithm.
//
// `n^2+1` can only be prime if `n^2` is an even number (technically that's not true for `n=1` but we know that `n >= 10`).
//
// Another significant insight is that 5 is the only prime where the last digit is 5.
// In order to ensure that `n^2+1`, `n^2+3`, `n^2+7` and `n^2+9` are prime (remember: `n^2` is even), the last digit of `n^2` must be a zero.
// And if the last digit of `n^2` is zero, then the last digit of `n` must be zero, too.
// Therefore I only need to look at all `n` which are multiples of 10 (''increment = 10'').
//
// My default implementation of ''isPrime'' performs a trial division against small primes because that's usually much faster than
// the full-blown Miller-Rabin test and already eliminates many candidates.
// However, since I have to check many numbers simultaneously, I made that trial division optional and perform it for all six potential primes at once.
// Only if all pass that test, then I start the Miller-Rabin test.
//
// I have to admit that initially my trial division only considered prime numbers up to 17. On the http://www.mathblog.dk blog I saw that using many more primes can be faster.
// That "trick" made my program about 10x faster because ''isPrime'' is called only 11552 times instead of more than 650000 times.
//
// # Hackerrank
// The "good" offsets 1, 3, 7, 9, 13, 27 are replaced by arbitrary six numbers (but each is smaller than 40).
// It took me some time to realize that these offsets can be either all even, too.
// The search space is reduced from 150 million to 10 million, too.
//
// I have no idea why one Hackerrank test case fails.
//
// # Alternatives
// There are many more optimizations which rely on a clever use of modulo but I'm not smart enough to provide a bullet-proof way ...

#include <iostream>
#include <vector>

#define ORIGINAL

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
bool isPrime(unsigned long long p, bool fastCheckAgainstSmallPrimes = true)
{
  // IMPORTANT: requires mulmod(a, b, modulo) and powmod(base, exponent, modulo)

  // some code from             https://ronzii.wordpress.com/2012/03/04/miller-rabin-primality-test/
  // with optimizations from    http://ceur-ws.org/Vol-1326/020-Forisek.pdf
  // good bases can be found at http://miller-rabin.appspot.com/

  // trivial cases
  if (fastCheckAgainstSmallPrimes)
  {
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
  }

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
  // generate all primes up to 500
  std::vector<unsigned int> primes = { 2 };
  for (unsigned int i = 3; i < 500; i += 2)
  {
    bool isPrime = true;

    // test against all prime numbers we have so far (in ascending order)
    for (auto x : primes)
    {
      // prime is too large to be a divisor
      if (x*x > i)
        break;

      // divisible => not prime
      if (i % x == 0)
      {
        isPrime = false;
        break;
      }
    }

    // yes, we have a prime
    if (isPrime)
      primes.push_back(i);
  }

  unsigned int tests = 1;
#ifndef ORIGINAL
  std::cin >> tests;
#endif
  while (tests--)
  {
    // largest number to be considered
    unsigned int limit = 150*1000000;
    std::cin >> limit;

    // always six consective increments where n^2 + good[i] must be prime
    std::vector<unsigned int> good = { 1, 3, 7, 9, 13, 27 }; // will be overwritten by Hackerrank input
#ifndef ORIGINAL
    for (auto& x : good)
      std::cin >> x;
#endif

    bool sameParity = true;
    unsigned int parity = good.front() % 2;
    for (auto x : good)
      sameParity &= x % 2 == parity;
    if (!sameParity)
    {
      std::cout << "0" << std::endl;
      continue;
    }

    // collect all numbers between 1 and the largest element of good which are not part of good
    // => n^2 + bad[i] must not be prime
    std::vector<unsigned int> bad;
    for (unsigned int i = parity; i < good.back(); i += 2)
    {
      bool isGood = false;
      for (auto x : good)
        isGood |= x == i;

      if (!isGood)
        bad.push_back(i);
    }

    // performance tweak: if good[] contains 1, 3, 7 and 9 then n must be a multiple of 10
    // or in other words: bad[0] = 5, bad[1] > 9
    unsigned int increment = 2;
    unsigned int start     = 1 - parity;
    if (bad.size() >= 2 && bad[0] == 5 && bad[1] > 9)
      increment = 10;

    // sum of all matching numbers
    unsigned long long sum = 0;
    for (unsigned int n = start; n < limit; n += increment)
    {
      auto square = n * (unsigned long long)n;

#ifdef ORIGINAL
      // n^2 + 3, n^2 + 7 and n^2 + 13 must be primes
      // => n^2 must not be divisible by 3, 7 or 13
      if (square % 3 == 0 || square % 7 == 0 || square % 13 == 0)
        continue;
#endif

      // check all numbers against small primes
      bool ok = true;
      for (auto p : primes)
      {
        for (auto check : good)
        {
          auto current = square + check;
          // not prime if remainder is zero
          if (current != p && current % p == 0)
          {
            // note: for n=10, current can be part of primes[],
            // that's why I check "current != p" to prevent false negatives
            ok = false;
            break;
          }
        }

        // failed (= not prime) ?
        if (!ok)
          break;
      }

      // use slower Miller-Rabin if still ok
      for (auto x : good)
        if (ok)
          ok &=  isPrime(square + x, false);
      for (auto x : bad)
        if (ok)
          ok &= !isPrime(square + x, true);

      // passed all tests ?
      if (ok)
        sum += n;
    }

    // display result
    std::cout << sum << std::endl;
  }

  return 0;
}
