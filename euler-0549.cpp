// ////////////////////////////////////////////////////////
// # Title
// Divisibility of factorials
//
// # URL
// https://projecteuler.net/problem=549
// http://euler.stephan-brumme.com/549/
//
// # Problem
// The smallest number `m` such that 10 divides `m!` is `m=5`.
// The smallest number `m` such that 25 divides `m!` is `m=10`.
//
// Let `s(n)` be the smallest number `m` such that `n` divides `m!`.
// So `s(10)=5` and s(25)=10`.
// Let `S(n)` be `sum{s(i)}` for `2 <= i <= n`.
// `S(100)=2012`.
//
// Find `S(10^8)`.
//
// # Solved by
// Stephan Brumme
// August 2017
//
// # Algorithm
// As always I wrote a simple function to solve the problem for small values.
// The function ''naive(n)'' returns `s(n)` and easily verifies that `s(2) + s(3) + s(4) + ... + s(100) = 2012`.
// Unfortunately it way too slow to find `s(10^8)` in a reasonable amount of time.
// (If you had enough spare time it would produce the correct result eventually).
//
// ''naive(n)'' computes `1! mod n`, then `2! mod n`, `3! mod n`, ... until it finds a value ''result'' such that `result! mod n == 0`.
//
// It took me a while to realize that
// `s(n) = s(p^{e_p} * other) = max(s(p^{e_p}), s(other) )`
//
// In plain English: if I factorize `n` into its prime factors `p_1`, `p_2`, ... then I only need to find a way to compute `s(p^{e_p})`
// where `e_p` is the exponent of the prime factors.
// An example:
// `24 = 2^3 * 3^1`
// `s(24) = s(2^3 * 3) = max(s(2^3), s(3)) = max(4, 3) = 4`
//
// Resolving the recursive structure of the formula shown above:
// `s(n) = max(s(p_1^{e_1}), s(p_2^{e_2}), s(p_3^{e_3}), ...)`
// where `p_i` are the prime factors of `n` and `e_i` the exponents of those prime factors.
//
// Obviously for each prime `p` we have `s(p^1) = p` because `p!` is the smallest factorial which contains `p` and thus can be divided by `p`.
// A modified version of the ''naive'' algorithm is pretty fast when it comes to finding `s(p^{e_p})`:
// instead of looking at each consecutive factorial `1!`, `2!`, `3!`, ... I only look at each factorial that is a multiple of `p`:
// `p! mod p^{e_p}`, `(2p)! mod p^{e_p}`, `(3p)! mod p^{e_p}`, ... until I find some `(x * p)! mod p^{e_p} == 0`.
// My ''cache'' contains only 2633 such values `prime^{2 ... x} < 10^8`.
//
// Whenever the ''main()'' function finds a prime number, then it adds its powers to the ''cache''.
// For all composite numbers it calls ''getSmallestFactorial'' which performs a prime factorization and returns the maximum value of any prime power encountered.
//
// # Note
// I'm surprised that this problem has a rating of only 10%. There are many easier problems with a higher percentage.
// In my personal opinion its rating should be something like 40%.
//
// Copying all prime numbers to a dense ''std::vector'' gives a little speed boost (almost 3x) in ''getSmallestFactorial'' at the cost of about 40 MByte RAM consumption.
// The high execution time (about 30 seconds) combined with an increased memory usage puts my solution in the top spot of the "[most expensive solutions](../performance/#slowandbig)" (as of August 2017) -
// I didn't expect that when I saw the 10% rating ...
//
// # Alternative
// It's possible to write faster solutions using some special properties of the Kempner function (see https://en.wikipedia.org/wiki/Kempner_function ).
// However, I wasn't aware of it and therefore didn't look it up.

#include <iostream>
#include <vector>
#include <unordered_map>
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

// ---------- problem specific code ----------

// compute all factorials until factorial % n == 0
unsigned int naive(unsigned int n)
{
  unsigned long long factorial = 1;
  unsigned int result = 0;
  while (factorial % n != 0)
  {
    result++;
    factorial *= result;
    factorial %= n;
  }
  return result;
}

// all prime numbers < 10^8
std::vector<unsigned int> primes;
// cache for i^2, i^3, i^4, ... where i is prime
std::unordered_map<unsigned int, unsigned int> cache;

// compute s(n)
unsigned int getSmallestFactorial(unsigned int n)
{
  // will be the result
  unsigned int best = 0;

  // split off all prime factors
  for (auto p : primes)
  {
    // p is not a prime factor of the current number ?
    if (n % p != 0)
      continue;

    // extract the current prime factor as often as possible
    // e.g. => 24 => 2^3 * 3 => primePower will be 8 and reduced = 3
    unsigned int primePower = 1;
    do
    {
      n          /= p;
      primePower *= p;
    } while (n % p == 0);

    // higher result ?
    best = std::max(best, cache[primePower]);

    // no further factorization possible ?
    if (n == 1)
      return best;
    if (isPrime(n))
      // s(prime) = prime
      return std::max(best, n);
  }

  return best;
}

int main()
{
  unsigned int limit = 100000000;
  std::cin >> limit;

  unsigned long long sum = 0;

  // simple algorithm, too slow
  //for (unsigned int i = 2; i <= 100; i++)
  //  sum += naive(i);

  // and now the more sophisticated approach

  // find all primes below 10^8
  fillSieve(limit);
  // copy those 5761455 primes to a dense array for faster access
  for (unsigned int i = 2; i < limit; i++)
    if (isPrime(i))
      primes.push_back(i);

  // find result for numbers with are powers of a single prime
  for (unsigned int i = 2; i <= limit; i++)
  {
    if (isPrime(i))
    {
      // pre-compute all values of i^2, i^3, ... where i is prime and store in cache[]
      unsigned long long power = i * (unsigned long long) i;
      for (unsigned int exponent = 2; power <= limit; exponent++)
      {
        // optimized version of naive(), skip i numbers in each iteration
        unsigned long long factorial = i;
        unsigned int result = i;
        do
        {
          result    += i;
          factorial *= result;
          factorial %= power;
        } while (factorial % power != 0);

        cache[power] = result;

        // next exponent
        power *= i;
      }

      // s(prime) = prime
      sum += i;
    }
    else
    {
      // compute s(non prime)
      sum += getSmallestFactorial(i);
    }
  }

  // and display the result
  std::cout << sum << std::endl;
  return 0;
}
