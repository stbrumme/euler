// ////////////////////////////////////////////////////////
// # Title
// Totient Chains
//
// # URL
// https://projecteuler.net/problem=214
// http://euler.stephan-brumme.com/214/
//
// # Problem
// Let `phi` be Euler's totient function, i.e. for a natural number `n`, `phi(n)` is the number of `k`, `1 <= k <= n`, for which `gcd(k,n) = 1`.
//
// By iterating `phi`, each positive integer generates a decreasing chain of numbers ending in 1.
// E.g. if we start with 5 the sequence 5,4,2,1 is generated.
// Here is a listing of all chains with length 4:
// 5,4,2,1
// 7,6,2,1
// 8,4,2,1
// 9,6,2,1
// 10,4,2,1
// 12,4,2,1
// 14,6,2,1
// 18,6,2,1
//
// Only two of these chains start with a prime, their sum is 12.
//
// What is the sum of all primes less than 40000000 which generate a chain of length 25?
//
// # Solved by
// Stephan Brumme
// June 2017
//
// # Algorithm
// I copied ''phi()'' from problem 70 (and removed the ''minQuotient'' parameter). Then I copied a prime sieve from my [toolbox](../toolbox/).
//
// The ''steps()'' function determines the totient chain length: it apply ''x = phi(x)'' until ''x'' is 1.
// I added two performance optimizations:
// - abort if chain length exceeds 25 (''maxSteps'')
// - if ''x'' is a power of two, that means only a single bit is set, then `phi(x) = log_2(x)` (shift right until that bit is in the right-most position)
//
// # Alternative
// Instead of chasing down the chains you can go the other way around:
// - set ''steps[0] = steps[1] = 1'', then ''steps[x] = steps[phi(x)]''
// However, this requires 40 MByte RAM and isn't much faster than my approach (which needs about 5 MByte).

#include <iostream>
#include <vector>

std::vector<unsigned int> primes;

// return Euler totient (taken from problem 70 and slightly modified)
unsigned int phi(unsigned int x)
{
  // totient function can be computed by finding all prime factors p
  // and subtracting them from x
  auto result  = x;
  auto reduced = x;
  for (auto p : primes)
  {
    // prime factors have to be p <= sqrt
    if (p*p > reduced)
      break;

    // not a prime factor ...
    if (reduced % p != 0)
      continue;

    // prime factors may occur multiple times, remove them all
    do
    {
      reduced /= p;
    } while (reduced % p == 0);

    // but subtract from result only once
    result -= result / p;
  }

  // we only checked prime factors <= sqrt(x)
  // there might exist one (!) prime factor > sqrt(x)
  // e.g. 3 is a prime factor of 6, and 3 > sqrt(6)
  if (reduced > 1)
    result -= result / reduced;

  return result;
}

// count length of totient chain, abort if more than maxSteps
unsigned int steps(unsigned int x, unsigned int maxSteps)
{
  unsigned int result = 1; // initial value is counted as a step, too

  // if x is prime, then phi(x) = x - 1
  x--;
  result++;

  // follow chain until we hit 1 or the chain becomes too long
  while (x > 1 && result < maxSteps)
  {
    // one more step ...
    x = phi(x);
    result++;

    // power of two ? (only a single bit set)
    if ((x & (x - 1)) == 0)
    {
      // simple chain for powers of two:
      // ... => 1024 => 512 => 256 => 128 => ... => 4 => 2 => 1
      while (x > 1)
      {
        x >>= 1;
        result++;
      }
    }
  }

  return result;
}

// sieve is copied from my [toolbox](../toolbox/)

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
  unsigned long long result = 0;

  unsigned int limit = 40000000;
  unsigned int maxSteps = 25;
  std::cin >> maxSteps >> limit;

  // start the prime sieve
  fillSieve(limit);

  // convert sieve to a "denser" data structure for phi(x)
  primes.push_back(2);

  for (unsigned int i = 3; i < limit; i += 2)
  {
    // check bits in sieve
    if (!isPrime(i))
      continue;

    // phi(i) needs only primes smaller than sqrt(i)
    if (i*i <= limit)
      primes.push_back(i);

    // primes need to be quite large to generate a long chain ...
    if (maxSteps == 25 && i < 9548417) // note: "magic constant" found in experiments
      continue;

    // compute chain
    unsigned int current = steps(i, maxSteps + 1);
    if (current == maxSteps)
      result += i;
  }

  std::cout << result << std::endl;
  return 0;
}
