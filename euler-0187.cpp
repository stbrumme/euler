// ////////////////////////////////////////////////////////
// # Title
// Semiprimes
//
// # URL
// https://projecteuler.net/problem=187
// http://euler.stephan-brumme.com/187/
//
// # Problem
// A composite is a number containing at least two prime factors. For example, `15 = 3 * 5`; `9 = 3 * 3`; `12 = 2 * 2 * 3`.
//
// There are ten composites below thirty containing precisely two, not necessarily distinct, prime factors: 4, 6, 9, 10, 14, 15, 21, 22, 25, 26.
//
// How many composite integers, `n < 10^8`, have precisely two, not necessarily distinct, prime factors?
//
// # Solved by
// Stephan Brumme
// May 2017
//
// # Algorithm
// My fast [prime sieve](../toolbox/#primesieve-eratosthenes) generates all prime numbers below `5 * 10^7` in about 120ms.
// Then all primes numbers are pairwise multiplied. I count the number of all products below `10^8`.
//
// # Alternative
// A prime counting function can solve this almost instantly (see my solution for problem 501).
//
// Or even simpler: ''prime[j]'' is the smallest prime to be multiplied with ''prime[i]''.
// Then ''limit / prime[i]'' will be the maximum number for ''prime[j]''. A simple binary search in ''primes'' can count how many primes are relevant.
// (that's prime counting, too, and equivalent to the speed-up trick in problem 501 for small ''n'').

#include <iostream>
#include <vector>
#include <algorithm>

// ---------- standard prime sieve from my toolbox ----------

// odd prime numbers are marked as "true" in a bitvector
std::vector<bool> sieve;

// return true, if x is a prime number
bool isPrime(unsigned int x)
{
  // handle even numbers
  if ((x & 1) == 0)
    return x == 2;

  // lookup for odd numbers
  return sieve[x >> 1];
}

// find all prime numbers from 2 to size
void fillSieve(unsigned int size)
{
  // store only odd numbers
  const unsigned int half = size >> 1;

  // allocate memory
  sieve.resize(half, true);
  // 1 is not a prime number
  sieve[0] = false;

  // process all relevant prime factors
  for (unsigned int i = 1; 2*i*i < half; i++)
    // do we have a prime factor ?
    if (sieve[i])
    {
      // mark all its multiples as false
      unsigned int current = 3*i+1;
      while (current < half)
      {
        sieve[current] = false;
        current += 2*i+1;
      }
    }
}

// ---------- and now problem-specific code ----------

int main()
{
  // generate all primes
  unsigned int limit = 100000000;
  unsigned int largestPrime = limit / 2 + 100; // at least one more prime than strictly needed
  fillSieve(largestPrime);

  // extract all prime numbers from sieve
  std::vector<unsigned int> primes = { 2 };
  primes.reserve(3002000); // avoid frequent re-allocations
  for (unsigned int i = 3; i < largestPrime; i += 2)
    if (isPrime(i))
      primes.push_back(i);

#define ORIGINAL
#ifndef ORIGINAL
  // Hackerrank has several test cases
  unsigned int tests = 1;
  std::cin >> tests;
  while (tests--)
#endif
  {
    std::cin >> limit;

    // compute all products of primes i and j where i <= j
    unsigned int count = 0;
    for (unsigned int i = 0; primes[i] * primes[i] < limit; i++)
      for (unsigned int j = i; primes[i] * primes[j] < limit; j++)
        // found one more solutions ...
        count++;

    std::cout << count << std::endl;
  }

  return 0;
}
