// ////////////////////////////////////////////////////////
// # Title
// Divisibility Multipliers
//
// # URL
// https://projecteuler.net/problem=274
// http://euler.stephan-brumme.com/274/
//
// # Problem
// For each integer `p > 1` coprime to 10 there is a positive divisibility multiplier `m < p` which preserves divisibility by `p`
// for the following function on any positive integer, `n`:
//
// f(n) = (all but the last digit of n) + (the last digit of n) * m
//
// That is, if `m` is the divisibility multiplier for `p`, then `f(n)` is divisible by `p` if and only if `n` is divisible by `p`.
//
// (When `n` is much larger than `p`, `f(n)` will be less than `n` and repeated application of `f` provides a multiplicative divisibility test for `p`.)
//
// For example, the divisibility multiplier for 113 is 34.
//
// `f(76275) = 7627 + 5 * 34 = 7797` : 76275 and 7797 are both divisible by 113
// `f(12345) = 1234 + 5 * 34 = 1404` : 12345 and 1404 are both not divisible by 113
//
// The sum of the divisibility multipliers for the primes that are coprime to 10 and less than 1000 is 39517.
// What is the sum of the divisibility multipliers for the primes that are coprime to 10 and less than 107?
//
// # Solved by
// Stephan Brumme
// August 2017
//
// # Algorithm
// Oops, I didn't understand the problem statement - and had to read it five times until I truly knew what Project Euler is asking for:
// - if `n` is a multiple of `p` then `f(n)` is a multiple of `p`, too
// - if `n` isn't a multiple of `p` then `f(n)` isn't a multiple of `p` as well
//
// A more formal way to describe `n` and `f(n)` is:
// (1) `n = 10a + b`
// (2) `f(n) = a + bm`
// If `n` is a multiple of `p`:
// (3) `n == 0 mod p` therefore `10a + b == 0 mod p`
// (4) `f(n) == 0 mod p` therefore `a + bm == 0 mod p`
//
// I picked a few random numbers and got lucky:
// (5) `10a + b == a + bm`
// (6) `n == a + bm`
// (7) `n - a == bm`
// (8) `dfrac{n - a}{b} == m`
// To my surprise the result was wrong for the sum of all prime `n` up to 1000 !
// It turns out I forgot that equations (5) to (8) are simply wrong because I replaced `f(n)` by `n` and they are missing `mod p`.
// That happens when I start thinking about a Project Euler problem after a long day at work ...
//
// Then I wrote the brute-force routine ''findM'' which iterates over several multiples of `p` until it finds some `m`.
// For performance reasons I abort after just 10 iterations - and this time the result is correct for `n < 1000` (even for bigger `n` but then ''findM'' becomes slow).
// An interesting observation was that equations (5) are actually working if the last digit is not 7 !
// I can't (and won't) prove that because I'm a software engineer and not a mathematician but there is probably an elegant way.
//
// So what's the problem with ''lastDigit = 7'' ?
// Let's start again and find some equations based on (1) to (4):
// (3) `10a + b == 0 mod p` (just repeated from above)
// (9) `10a == -b mod p`
//
// (4) `a + bm == 0 mod p`  (just repeated from above)
// (10) `10 * (a + bm) == 0 mod p`
// (11) `10a + 10bm == 0 mod p`
//
// Merging (9) and (11):
// (12) `-b + 10bm == 0 mod p`
// (13) `b * (10m - 1) == 0 mod p`
//
// This should be valid for every `b`, even for `b = 1`:
// (14) `10m - 1 == 0 mod p`
// (15) `10m = 1 mod p`
//
// If `m` is the modular inverse of `10 mod p` then `f(n)` is a multiple of `p`.
// So it all boils down to adding the modular inverses of 10 for each `n`.
//
// There are multiple way to find the modular inverse. In problem 381 I implemented Fermat's little theorem and the Extended Euclidean algorithm.
// I copied the former (which means I copied ''powmod'' from my toolbox and then the modular inverse is ''powmod(10, n-2, n)'').
//
// # Note
// The Extended Euclidean algorithm is faster than Fermat's little theorem. However, ''powmod'' is part of so many solutions that I kind of like it.
// And I could write it's code instantly (if I lost my toolbox) whereas I would need to look up the Extended Euclidean algorithm.
//
// # Alternative
// After submitting my solution I saw some astonishingly simple solutions exploiting the repeated nature of modulo.

#include <iostream>
#include <vector>

// uncomment to run only the slow algorithm
//#define SLOW
// uncomment to revert to fast formula for last digit != 7
#define TRICK7
// if both #defines are not used, then only the inverse modular computation is started

// brute-force way to find the unique m
// see below for more efficient ways
unsigned int findM(unsigned int p)
{
  // every multiple of p must fulfil the equations
  // kp = 10a + b  (split that multiple into the first digits and the last digit)
  // (a + mb) % p == 0
  for (unsigned int m = 1; m < p; m++)
  {
    // there are a few "false" positives when k = 1 and b = 7
    // but they don't survive when testing against more multiples
    const auto AtLeastMultiples = 10; // number was chosen quite arbitrary

    bool ok = true;
    // test the first 10 multiples of p
    for (auto k = 1; k < AtLeastMultiples; k++)
    {
      // split multiple
      auto current = k * p;
      auto a = current / 10;
      auto b = current % 10;
      // each multiple is divisible by p but is a + mb, too ?
      if ((a + m * b) % p != 0)
      {
        ok = false;
        break;
      }
    }

    // found a match
    if (ok)
      return m;
  }

  // never reached
  return 0;
}

// return (base^exponent) % modulo for 32-bit values, no need for mulmod
// copied from my toolbox
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

int main()
{
  unsigned int limit = 10000000;
  std::cin >> limit;

  // store only odd numbers (based on standard prime sieve from my toolbox)
  // odd prime numbers are marked as "true" in a bitvector
  std::vector<bool> sieve(limit, true);

  unsigned long long sum = 0;
  for (unsigned int n = 3; n < limit; n += 2)
  {
    // accept only primes
    if (!sieve[n >> 1])
      continue;

    // remove multiples if n < sqrt(limit)
    if (n * (unsigned long long)n < limit)
      for (auto i = n * n; i < limit; i += 2*n)
        sieve[i >> 1] = false;

    // not coprime with 10 => reject 2 and 5 (outer loop already ignores 2)
    if (n == 5)
      continue;

    // f(n) = firstDigits       + lastDigit * m
    //      = (n - n % 10) / 10 + (n % 10)  * m
    auto lastDigit   = n % 10;
    auto firstDigits = n / 10;

#ifdef SLOW
    // use only brute-force algorithm
    sum += findM(n);
    continue;
#endif

#ifdef TRICK7
    // surprisingly, this simple formula works whenever lastDigit is not 7
    auto reduced = (n - firstDigits) / lastDigit;
    if (lastDigit != 7)
    {
      sum += reduced;
      continue;
    }
#endif

    // for lastDigit=7 I have to find the modular inverse of 10 mod i
    // note: this works for every other lastDigit, too,
    //       but is much slower than the straightforward formula above

    // Fermat's Little Theorem: a^p     == a mod p
    // divide both sides by a:  a^(p-1) == 1 mod p
    // once more:               a^(p-2) == a^-1 mod p
    // where a^-1 is the inverse of a => equal to a^(p-2) % p
    // in my case, a=10 and p=n
    // thus I need modularInverse(10, n) = powmod(10, n - 2, n)
    auto modularInverse = powmod(10, n - 2, n);
    sum += modularInverse;
  }

  // print result
  std::cout << sum << std::endl;
  return 0;
}
