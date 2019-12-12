// ////////////////////////////////////////////////////////
// # Title
// Maximum number of divisors
//
// # URL
// https://projecteuler.net/problem=485
// http://euler.stephan-brumme.com/485/
//
// # Problem
// Let `d(n)`   be the number of divisors of `n`.
// Let `M(n,k)` be the maximum value of `d(j)` for `n <= j <= n+k-1`.
// Let `S(u,k)` be the sum of `M(n,k)` for `1 <= n <= u-k+1`.
//
// You are given that `S(1000, 10) = 17176`.
//
// Find `S(100 000 000, 100 000)`.
//
// # Solved by
// Stephan Brumme
// December 2017
//
// # Algorithm
// I needed to translate the short problem description into plain English (well, I'm German so actually it wasn't English ...):
// to find `S(1000, 10)` I have to create 990 blocks with 10 elements each:
// `\{ d(1), d(2), ..., d(10) \}`,
// `\{ d(2), d(3), ..., d(11) \}`,
// `\{ d(3), d(4), ..., d(12) \}`,
// ...
// `\{ d(991), d(992), ..., d(1000) \}`
// and add the maximum values of all those 990 blocks.
//
// My first step is to fill the container ''numDivisors'' with `d(1)`, `d(2)`, ..., `d(10^8)`.
// The maximum value is 768 so an ''unsigned short'' is sufficient - but it still requires 200 MByte RAM.
//
// I implemented two algorithms to compute the number of divisors of `n`:
// ''countDivisorsSlow'' is based on trial division: it divides `n` by every number between `1` and `n` and counts how often the remainder is zero.
// That's pretty fast if `n` is small (well, even `10^6` is almost okay) but much too slow for `10^8`.
//
// ''countDivisors'' needs only one second to perform the same task:
// if the number `n` is factorized into its prime factors `n = {p_1}^e_1 * {p_2}^e_2 * {p_3}^e_3 * ...`
// then the number of divisors is `d(n) = (e_1 + 1) * (e_2 + 1) * (e_3 + 1) * ...`
// My first step is to find all prime numbers below `10^8` (see below for a significant optimization / chapter "Note").
// Then all `d(n)` are initialized with 1 and:
// - iterate over all multiples `m` of `2^1 =  2`, multiply `d(m)` by `1+1=2`
// - iterate over all multiples `m` of `2^2 =  4`, multiply `d(m)` by `2+1=3` and divide by `1+1=2` (that's an undo of the previous step)
// - iterate over all multiples `m` of `2^3 =  8`, multiply `d(m)` by `3+1=4` and divide by `2+1=3` (that's an undo of the previous step)
// - iterate over all multiples `m` of `2^4 = 16`, multiply `d(m)` by `4+1=5` and divide by `3+1=4` (that's an undo of the previous step)
// - ... and so on, until `2^x > 10^8`
// - repeat the same procedure for all other primes 3, 5, 7, 11, ...
//
// Now that I have `d(n)`, I need to find the maximums of each blocks. I wrote two algorithms to find them:
// - the obvious ''bruteForce()'' algorithm with two nested loops has `10^8 * 10^5 = 10^13` iterations and is too slow.
// - a smarter algorithm can do the same job in just `10^8` iterations !
//
// In ''search()'', each iteration updates ''mostRecent[d(n)]'' with the current position ''n'' (and enlarges ''mostRecent'' if required).
// Zero is more or less a dummy element and the whole algorithm still works if I omit it but it simplifies index calculations a lot.
// Only once it causes minor problems: the maximum number is the size of ''mostRecent'' minus 1.
//
// For example:
//
// ||  3 ||   3  ||   10           ||
// ||! n ++ d(n) ++ mostRecent     ||
// ||  0 ++   0  ++ { 0 }          ||
// ||  1 ++   1  ++ { 0,1 }        ||
// ||  2 ++   2  ++ { 0,1,2 }      ||
// ||  3 ++   2  ++ { 0,1,3 }      ||
// ||  4 ++   3  ++ { 0,1,3,4 }    ||
// ||  5 ++   2  ++ { 0,1,5,4 }    ||
// ||  6 ++   4  ++ { 0,1,5,4,6 }  ||
// ||  7 ++   2  ++ { 0,1,7,4,6 }  ||
// ||  8 ++   4  ++ { 0,1,7,4,8 }  ||
// ||  9 ++   3  ++ { 0,1,7,9,8 }  ||
// || 10 ++   4  ++ { 0,1,7,9,10 } ||
//
// If an element of ''mostRecent'' is too far away, that means if its value is more than ''blockSize'' away from the current position, then it becomes invalid.
// That's only important for the largest indices of ''mostRecent'': they will be removed, thus shrinking ''mostRecent''.
//
// # Note
// `d(n)` is a large number if:
// - `n` has many different prime factors or
// - those prime factors have large exponents
// - or both
// One of the largest values of `d(n)` is `d(73513440) = 768` because `73513440 = 2^5 * 3^3 * 5 * 7 * 11 * 13 * 17`
// such that `d(73513440) = (5+1) * (3+1) * (1+1) * (1+1) * (1+1) * (1+1) * (1+1) = 6 * 4 * 2 * 2 * 2 * 2 * 2 = 768`
//
// In conclusion, pretty much every maximum `d(n)` of a block has typically very small prime factors.
// Assuming that all prime factors of a maximum `d(n)` are less than `sqrt{10^8} = 10^4`, my code will still find the correct result for `S(10^8, 10^5)`.
// However, this "square-root" assumption isn't always true: it fails for `S(1000, 10)` (the problem is the small ''blockSize'').
//
// The less primes I use as prime factors in ''countDivisors'' the faster the code becomes (at the risk of being off, depending on ''blockSize'').
// I manually looked for the smallest "valid" value for ''primeLimit'' for `S(10^8, 10^5)` and it turned out to be 107.
// Using this parameter cuts down the execution time from 1.8 to 1.1 seconds.
//
// # Alternative
// The whole algorthm could be rewritten to process the data in chunks instead of all 100 million at once.
// Memory consumption will drop considerably and I expect performance to remain about the same (maybe a tiny bit slower).
// But the code size will grow considerably and become less reabable.

#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

// store the number of divisors for the first 100 million numbers
typedef unsigned short Number; // => 200 MByte
std::vector<Number> numDivisors;

// slow trial division
void countDivisorsSlow(unsigned int limit)
{
  // zero has no divisors
  numDivisors = { 0 };

  // process all numbers 1..10^8
  for (unsigned int current = 1; current <= limit; current++)
  {
    Number count = 0;
    // trial division of all numbers <= sqrt(current)
    for (unsigned int divisor = 1; divisor*divisor <= current; divisor++)
    {
      // divisible ?
      if (current % divisor != 0)
        continue;

      count++;                        // one divisor if i^2 = x
      if (divisor*divisor != current) // or two if not (it's i and x/i)
        count++;
    }

    numDivisors.push_back(count);
  }
}

// similar to a prime sieve, much faster
void countDivisors(unsigned int limit, unsigned int primeLimit = 0)
{
  numDivisors.resize(limit + 1, 1);
  // zero has no divisors
  numDivisors[0] = 0;

  // accurate algorithm by default
  if (primeLimit == 0)
    primeLimit = limit;

  // simple prime sieve (trial division because it has the shortest/most simple implementation)
  std::vector<unsigned int> primes = { 2 };
  for (unsigned int smallPrime = 3; smallPrime <= primeLimit; smallPrime += 2)
  {
    // find primes by trial division
    bool isPrime = true;
    for (auto p : primes)
      if (smallPrime % p == 0)
      {
        isPrime = false;
        break;
      }

    // yes, found another prime
    if (isPrime)
      primes.push_back(smallPrime);
  }

  // multiply all "simple" multiples of each prime by 2
  // ... and multiply all multiples p^2 by 3
  // ... and multiply all multiples p^3 by 4
  // ... and so on
  for (auto p : primes)
  {
    // "simple" multiples
    for (auto i = p; i <= limit; i += p)
      numDivisors[i] *= 2;

    // multiples of p^2, p^3, ...
    auto power    = p * p;
    auto exponent = 2;
    while (power <= limit)
    {
      // undo the previous factor and multiply with "better" factor
      for (auto i = power; i <= limit; i += power)
        numDivisors[i] = (numDivisors[i] / exponent) * (exponent + 1);

      // from to p^2 to p^3 to p^4 ...
      power *= p;
      exponent++;
    }
  }
}

// enough to verify the example ... (you need to fill numDivisors first)
unsigned long long bruteForce(unsigned int limit, unsigned int blockSize)
{
  // for each range ...
  unsigned long long result = 0;
  for (unsigned int from = 1; from <= limit - blockSize + 1; from++)
  {
    // ... find its maximum
    Number maximum = numDivisors[from];
    for (unsigned int i = 1; i < blockSize; i++)
      maximum = std::max(maximum, numDivisors[from + i]);

    // and add it to the result
    //std::cout << maximum << " ";
    result += maximum;
  }

  return result;
}

// fast O(n) algorithm: a single pass iterating over all 10^8 elements of numDivisors (you need to fill numDivisors first)
unsigned long long search(unsigned int limit, unsigned int blockSize)
{
  // store the most recent position a certain number of divisors was encountered
  // actually the maximum is 768 divisors (first seen at 73513440)
  std::vector<unsigned int> mostRecent;

  // process number of divisors of the first block (10^5-1 elements)
  for (unsigned int i = 0; i < blockSize; i++)
  {
    // new/updated maximum
    auto current = numDivisors[i];
    if (current >= mostRecent.size())
      mostRecent.resize(current + 1, 0);
    mostRecent[current] = i;
  }

  unsigned long long result = 0;
  for (unsigned int i = blockSize; i <= limit; i++)
  {
    // remove "old" maximums
    auto tooFar = i - blockSize;
    while (!mostRecent.empty() && mostRecent.back() <= tooFar)
      mostRecent.pop_back();

    // new/updated maximum
    auto current = numDivisors[i];
    if (current >= mostRecent.size())
      mostRecent.resize(current + 1, 0);
    mostRecent[current] = i;

    // highest index is equal to size-1
    result += mostRecent.size() - 1;
  }

  return result;
}

int main()
{
  unsigned int limit     = 100000000; // 10^8
  unsigned int blockSize =    100000; // 10^5
  std::cin >> limit >> blockSize;

  // compute number of divisors for all 100 million numbers
  unsigned int primeLimit = limit;
  // faster heuristic if possible, fails with small blockSize
  if (blockSize >= 100)
    primeLimit = sqrt(limit);
  // lowest limit for default input: I found it by trial'n'error
  if (limit == 100000000 && blockSize == 100000)
    primeLimit = 107;

  // compute number of divisors (might be off a little bit when primeLimit != limit)
  countDivisors(limit, primeLimit);

  // and now find S(10^8, 10^5)
  //std::cout << bruteForce(limit, blockSize) << std::endl;
  std::cout << search(limit, blockSize) << std::endl;
  return 0;
}
