// ////////////////////////////////////////////////////////
// # Title
// Prime triples and geometric sequences
//
// # URL
// https://projecteuler.net/problem=518
// http://euler.stephan-brumme.com/518/
//
// # Problem
// Let `S(n) = sum{a+b+c}` over all triples `(a,b,c)` such that:
//
// - `a`, `b`, and `c` are prime numbers.
// - `a < b < c < n`.
// - `a+1`, `b+1`, and `c+1` form a geometric sequence.
//
// For example, `S(100) = 1035` with the following triples:
//
// `(2, 5, 11)`, `(2, 11, 47)`, `(5, 11, 23)`, `(5, 17, 53)`, `(7, 11, 17)`,
// `(7, 23, 71)`, `(11, 23, 47)`, `(17, 23, 31)`, `(17, 41, 97)`, `(31, 47, 71)`, `(71, 83, 97)`
//
// Find `S(10^8)`.
//
// # Solved by
// Stephan Brumme
// September 2017
//
// # Algorithm
// This problem is somehow similar to problem 141:
// I learnt from the 141 forum that a geometric progression can be represented as `kyy`, `kxy`, `kxx` where `x > y` and `gcd(x, y) = 1`
// I wrote three nested loops for `k`, `x` and `y` which check whether `kxx - 1`, `kxy - 1` and `kyy - 1` are prime numbers.
//
// My code contains a few optimizations:
// - the loop nesting order `x`, `k`, `y` is roughly 30% faster than the more obvious `x`, `y`, `k`
// - I deferred the `gcd()` test because of its slow modulo computations
// - ... but early on do a quick bit test whether `x` and `y` are both even ==> because that implies `gcd(x, y) > 1`
//
// # Note
// You will find a little "debugging helper", too: I implemented a simple brute-force algorithm (see ''bruteForce'') which helped me
// to locate two bugs I had in my first version of ''count'' (which is the final/fast version).

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
  const unsigned int half = (size >> 1) + 1;

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

// greatest common divisor
template <typename T>
T gcd(T a, T b)
{
  while (a != 0)
  {
    T c = a;
    a = b % a;
    b = c;
  }
  return b;
}

// ---------- problem specific code ----------

// slow approach
unsigned long long bruteForce(unsigned int limit)
{
  unsigned long long sum = 0;

  // look for geometric progression a+1, b+1, c+1 where all three are prime numbers
  // iterate over all prime numbers a and c where a < c
  for (unsigned int a = 2; a < limit; a++)
  {
    if (!isPrime(a))
      continue;

    for (auto c = a + 4; c < limit; c++)
    {
      if (!isPrime(c))
        continue;

      // (b+1)^2 = (a+1) * (c+1)
      auto b2 = (a + 1) * (c + 1);
      unsigned int b = sqrt(b2);
      if (b * b != b2)
        continue;
      b--;

      // of course: b must be prime, too
      if (!isPrime(b))
        continue;

      // found one more solution
      std::cout << a << " " << b << " " << c << std::endl;
      sum += a + b + c;
    }
  }

  return sum;
}

// find triples k*x*x, k*x*y, k*y*y whose predecessors are prime
unsigned long long count(unsigned int limit)
{
  unsigned long long sum = 0;

  // I tried a few variations and the following order of loops turned out to be the fastest:
  // x => k => y
  // the more obvious order x => y => k is about 30% slower on my computer
  for (unsigned int x = 2; x*x < limit; x++)
    for (unsigned int k = 1; k*x*x < limit; k++)
    {
      auto a = k*x*x - 1;
      if (!isPrime(a))
        continue;

      for (unsigned int y = 1; y < x; y++)
      {
        // performance tweak: if both x and y are even then the gcd() test will fail
        if ((x & 1) == 0 && (y & 1) == 0)
          y++;

        auto b = k*x*y - 1;
        auto c = k*y*y - 1;
        if (!isPrime(b))
          continue;
        if (!isPrime(c))
          continue;

        // avoid duplicate solutions
        if (gcd(x, y) > 1)
          continue;
        // note: I delayed this gcd() for performance reasons
        //       it's more "expensive" to check right after the beginning of the y-loop

        // found one more solution
        sum += a + b + c;
      }
    }

  return sum;
}

int main()
{
  unsigned int limit = 100000000;
  std::cin >> limit;

  fillSieve(limit);
  //std::cout << bruteForce(limit) << std::endl;
  std::cout << count(limit) << std::endl;

  return 0;
}
