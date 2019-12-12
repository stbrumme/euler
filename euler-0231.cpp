// ////////////////////////////////////////////////////////
// # Title
// The prime factorisation of binomial coefficients
//
// # URL
// https://projecteuler.net/problem=231
// http://euler.stephan-brumme.com/231/
//
// # Problem
// The binomial coefficient `^10 C_3 = 120`.
// `120 = 23 * 3 * 5 = 2 * 2 * 2 * 3 * 5`, and `2 + 2 + 2 + 3 + 5 = 14`.
// So the sum of the terms in the prime factorisation of `^10 C_3` is 14.
//
// Find the sum of the terms in the prime factorisation of `^20000000 C_15000000`.
//
// # Solved by
// Stephan Brumme
// June 2017
//
// # Algorithm
// Aside from a standard prime sieve, the main work is done in ''add(n)'':
//
// The binomial coefficient is: `^n{C_k} = {{n}choose{k}} = dfrac{n!}{(n-k)! k!}`
// where the factorial is `n! = 1 * 2 * 3 * 4 * ... * n`, e.g. `10! = 1 * 2 * 3 * 4 * ... * 10`.
//
// However, the problem statement doesn't ask for the factorial but the sum of prime factors of all elements in the factorial sequence.
// That means `add(10!) = 1 + 2 + 3 + (2 + 2) + 5 + (2 + 3) + 7 + (2 + 2 + 2) + (3 + 3) + (2 + 5) = 45`.
// The main insight here is that `add(a * b) = add(a) + add(b)`.
//
// You can see that every second number contains prime factor 2, every third prime factor 3, every fifth prime factor 5 ... and so on.
// There are `\lfloor frac{10}{2} \rfloor = 5` elements which contain prime factor 2 at least once. That's a sum of `2 * 5 = 10`.
// There are `\lfloor frac{10}{2^2} \rfloor = 2` elements which contain prime factor 2 at least twice. That's a sum of `2 * 2 = 4` on top.
// There are `\lfloor frac{10}{2^3} \rfloor = 1` elements which contain prime factor 2 at least three times. That's a sum of `2 * 1 = 2` on top.
// The sum of all prime factors 2 is `10 + 4 + 2 = 16`.
//
// Following the same logic, but this time prime factor 3:
// There are `\lfloor frac{10}{3} \rfloor = 3` elements which contain prime factor 3 at least once. That's a sum of `3 * 3 = 9`.
// There are `\lfloor frac{10}{3^2} \rfloor = 1` elements which contain prime factor 3 at least twice. That's a sum of `3 * 1 = 3`.
// The sum of all prime factors 3 is `9 + 3 = 12`.
//
// Now the algorithm becomes apparent:
// - iterate over all potential prime factors ''p''
// - compute how many numbers contain ''p'',   add ''p * count'' to the sum
// - compute how many numbers contain ''p^2'', add ''p * count'' to the sum
// - compute how many numbers contain ''p^3'', add ''p * count'' to the sum
// - ...
//
// # Alternative
// My first approach was to generate a huge look-up table:
// - if a number ''i'' is a prime number, set ''sum[i] = i''
// - then for each number ''i'', multiply it by all primes ''p'' and set ''sum[i * p] = sum[i] + p''
// - add all ''sums[]'' up to ''i'', that's simply ''sums[i] += sums[i - 1]'' because all ''i'' are processed in increasing order
//
// That way more than 160 MByte RAM were used - which would have been a new record for my Project Euler solutions.
// I felt very uncomfortable of that memory consumption for such a "small" problem. And it was about 10x slower.
// So I spent a whole commuter ride thinking about the problem and came up with the current solution ...
// Note: read access to that precomputed table is extremely fast ! If you need to query heaps of factorial prime sums then
// my first approach might be an actually viable solution.

#include <iostream>
#include <vector>

#include <vector>

// odd prime numbers are marked as "true" in a bitvector
std::vector<bool> sieve;
// collection of all primes from the sieve (I do this because it's faster)
std::vector<unsigned int> primes = { 2 };

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

// ----- here the main algorithm starts -----

// return sum of all prime factors of n!
unsigned long long add(unsigned int n)
{
  unsigned long long sum = 0;
  for (auto p : primes)
  {
    // prime too large ? => done
    if (p > n)
      return sum;

    // start with p^1, then p^2 in next iteration
    unsigned long long multipleP = p;
    // initial count
    unsigned long long count = n / multipleP;
    do
    {
      // adjust result
      sum += p * count;

      // increase exponent by one, that means p^i => p^(i+1)
      multipleP *= p;
      // update count for next iteration
      count = n / multipleP;
    } while (count > 0); // multipleP > n
  }
  return sum;
}

// ----- below is my first approach -----

// my first attempt: compute a huge lookup table where
// (code not used anymore)
std::vector<unsigned long long> sums;
void generateTable(unsigned int limit)
{
  sums.resize(limit + 1, 0);
  for (unsigned int i = 2; i < sums.size(); i++)
  {
    // prime number ? its only prime factor is the number itself
    if (sums[i] == 0)
      sums[i] = i;

    // multiply with all prime numbers
    for (auto p : primes)
    {
      // too large ?
      if (i * p >= sums.size())
        break;

      // add current prime factor
      sums[i * p] = sums[i] + p;
    }

    // add all prime factors of (i-1)! to i
    // => that's the sum of prime factors of i!
    sums[i] += sums[i - 1];
  }
}

int main()
{
  // read input values n >= k
  unsigned int n = 20000000;
  unsigned int k = 15000000;
  std::cin >> n >> k;

  // generate all prime numbers
  fillSieve(n);

  // extract prime numbers
  for (unsigned int i = 3; i <= n; i += 2)
    if (isPrime(i))
      primes.push_back(i);

  std::cout << add(n) - (add(n - k) + add(k)) << std::endl;

  // older, slower algorithm
  //generateTable(n);
  //std::cout << sums[n] - (sums[n - k] + sums[k]) << std::endl;

  return 0;
}
