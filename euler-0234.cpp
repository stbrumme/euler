// ////////////////////////////////////////////////////////
// # Title
// Semidivisible numbers
//
// # URL
// https://projecteuler.net/problem=234
// http://euler.stephan-brumme.com/234/
//
// # Problem
// For an integer `n >= 4`, we define the lower prime square root of `n`, denoted by `lps(n)`, as the largest prime `<= sqrt{n}`
// and the upper prime square root of `n`, `ups(n)`, as the smallest prime `>= sqrt{n}`.
//
// So, for example, `lps(4) = 2 = ups(4)`, `lps(1000) = 31`, `ups(1000) = 37`.
// Let us call an integer `n >= 4` semidivisible, if one of `lps(n)` and `ups(n)` divides `n`, but not both.
//
// The sum of the semidivisible numbers not exceeding 15 is 30, the numbers are 8, 10 and 12.
// 15 is not semidivisible because it is a multiple of both `lps(15) = 3` and `ups(15) = 5`.
// As a further example, the sum of the 92 semidivisible numbers up to 1000 is 34825.
//
// What is the sum of all semidivisible numbers not exceeding 999966663333 ?
//
// # Solved by
// Stephan Brumme
// August 2017
//
// # Algorithm
// My standard prime sieve from my [toolbox](/toolbox/) is extremely quick in finding all prime numbers below `sqrt{999966663333} approx 10^6`.
//
// I always look at two consecutive primes numbers ''last'' and ''next''.
// Numbers between `last^2` and `next^2` are semidivisible if they are either a multiple of `last` or `next` but not both.
// Similar to the very first problem at Project Euler (problem 1) I count all multiples of `last` and `next` and subtract all multiples of `last * next`.
// I tried to find a closed form but they all looked rather messy. The simple ''for''-loops perform the same task in less than 0.02 seconds.
//
// The hardest part was to get the borders right, especially the last interval from `999983^2 = 999966000289` to `999966663333`.
// That's why there are (too many ?) checks against ''limit''.

#include <iostream>
#include <vector>
#include <cmath>

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

int main()
{
  unsigned long long limit = 999966663333ULL;
  std::cin >> limit;
  fillSieve(sqrt(limit) + 100); // a few more primes than strictly necessary ...

  // sum of all matches
  unsigned long long sum = 0;

  // 2 is the smallest prime
  unsigned long long last = 2;
  // until lps(last^2) exceeds the limit
  while (last*last <= limit)
  {
    // find next prime
    auto next = last + 1;
    while (!isPrime(next))
      next++;

    // note: for each square of a prime lps(p^2) = ups(p^2) = p
    //       => p^2 is obviously divisible by both and hence can't be semidivisible

    // all loops run from "from" and "to" but skip both (because not semidivisible)
    auto from = last * last;
    auto to   = next * next;

    // the following loops could be replaced by clever arithmetic
    // but they are super-fast and most of the time is spent sieving primes anyway

    // divisible by lps(i)
    for (auto i = from + last; i < to && i <= limit; i += last)
      sum += i;

    // don't exceed the limit
    while (to - next > limit)
      to -= next;
    // divisible by ups(i)
    for (auto i = to - next; i > from; i -= next)
      sum += i;

    // but not those divisible by both
    for (auto i = from - from % (last * next); i < to && i <= limit; i += last * next)
    {
      if (i <= from)
        continue;
      if (i > limit)
        break;

      // counted twice, remove both
      sum -= i + i;
    }

    // next pair of primes
    last = next;
  }

  // display result
  std::cout << sum << std::endl;
  return 0;
}
