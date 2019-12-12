// ////////////////////////////////////////////////////////
// # Title
// Largest integer divisible by two primes
//
// # URL
// https://projecteuler.net/problem=347
// http://euler.stephan-brumme.com/347/
//
// # Problem
// The largest integer `<= 100` that is only divisible by both the primes 2 and 3 is 96, as 96=32*3=25*3.
// For two distinct primes `p` and `q` let `M(p,q,N)` be the largest positive integer `<=N` only divisible by both `p` and `q`
// and `M(p,q,N)=0` if such a positive integer does not exist.
//
// E.g. `M(2,3,100)=96`.
// `M(3,5,100)=75` and not 90 because 90 is divisible by 2, 3 and 5.
// Also M(2,73,100)=0 because there does not exist a positive integer `<= 100` that is divisible by both 2 and 73.
//
// Let `S(N)` be the sum of all distinct `M(p,q,N)`. `S(100)=2262`.
//
// Find `S(10 000 000)`.
//
// # Solved by
// Stephan Brumme
// July 2017
//
// # Algorithm
// I go through all pairs `(i,j)` of primes `i` and `j` where `i < j`.
// Then i analyze `i^1`: find the largest `j^k_1` such that `i^1 * j^k <= limit`
// In the second iteration I find the largest `j^{k_2}` such that `i^2 * j^{k_2} <= limit`
// ... and so on. Each time I check whether the product exceeds any previous product.
//
// About 50% is comprised of the standard prime sieve from my [toolbox](/toolbox/).

#include <iostream>
#include <vector>

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

// ---------- problem-specific code ----------

int main()
{
  unsigned long long limit = 10000000;
  std::cin >> limit;

  // compute all prime numbers
  fillSieve(limit / 2);

  // will contain the result
  unsigned long long sum = 0;

  // first prime
  for (unsigned long long i = 2; i*i <= limit; i++)
  {
    // primes only
    if (!isPrime(i))
      continue;

    // second prime
    unsigned long long j = (i == 2) ? 3 : i + 2; // somewhat tricky way to express: "next odd number, start with 3"
    for (; i*j <= limit; j += 2)
    {
      // primes only
      if (!isPrime(j))
        continue;

      // largest number expressed as i^something * j^somethingelse (<= limit)
      unsigned long long maxProduct = 0;

      // note: j needs to be long long because otherwise a few overflows
      //       lead to a result which is slightly off

      // i^1 * j^1
      auto product = i * j;
      // for i^1, i^2, i^3, ... find the maximum exponent for j
      do
      {
        // increase exponent of j as much as possible
        auto current = product;
        while (current * j <= limit)
          current *= j;

        // better than before ?
        if (maxProduct < current)
          maxProduct = current;

        // increment i's exponent by one
        product *= i;
      } while (product <= limit);

      // add all maximum products
      sum += maxProduct;
    }
  }

  // display result
  std::cout << sum << std::endl;
  return 0;
}
