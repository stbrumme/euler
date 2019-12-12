// ////////////////////////////////////////////////////////
// # Title
// Idempotents
//
// # URL
// https://projecteuler.net/problem=407
// http://euler.stephan-brumme.com/407/
//
// # Problem
// If we calculate `a^2 mod 6` for `0 <= a <= 5` we get: 0,1,4,3,4,1.
//
// The largest value of `a` such that `a^2 == a mod 6` is `4`.
// Let's call `M(n)` the largest value of `a < n` such that `a^2 == a (mod n)`.
// So `M(6) = 4`.
//
// Find `sum{M(n)}` for `1 <= n <= 10^7`.
//
// # Solved by
// Stephan Brumme
// August 2017
//
// # Algorithm
// The function ''slow()'' just iterates over all `a` - that's nice for small `n` but obviously to slow for large `n`.
// However, it reveals a few properties:
// - `a` is often close to `n`
// - if `n` is a prime number then `a = 1`
// - if `n` has only one prime factor, that means is a perfect square/cube/... of a prime then `a = 1` as well
// Actually, those small solutions can be found online, too: https://oeis.org/A182665
//
// Let's rewrite the equation `a^2 == a mod n`
// (1) `a^2 - a == 0 mod n`
// (2) `a(a-1)  == 0 mod n`
//
// The program has to find the largest `a < n` where `a(a-1)` is a multiple of `n`.
// Each prime factor of `n` has to be either a prime factor of `a` or `a - 1`.
//
// My program precomputes the largest prime factor of each `n` and stores it in ''largestPrime'' (see ''findLargestPrime'').
// Whenever ''search(n)'' is started, it looks up the largest prime factor of ''n''.
// If ''largestPrime[n] == n'' then either `n` is prime or a power of a prime (e.g. `32 = 2^5` therefore ''largestPrime[32] = 32'').
// Strictly speaking I don't need that special treatment for powers of a prime but it gives a little performance boost.
//
// Then I analyze all multiples `x` of `n`'s largest prime `p`, beginning with the largest multiple:
// - if `x = a` is a multiple of `p` then check whether `x^2 == x mod n` is true ==> if yes, then `x` is the largest `a`
// - if `x = a - 1` ==> `x + 1 = a` is a multiple of `p` then check `(x + 1)^2 == x mod n` is true ==> if yes, then `x + 1` is the largest `a`
//
// # Alternatives
// My solution is pretty basic. Some more advanced tricks with modular arithmetic can find the result about 10x faster.
// But my knowledge about modular arithmetic is still very limited - hence I didn't spot these shortcuts.
//
// # Note
// I could reduce memory consumption by 50% if ''largestPrime[n] = n / largestPrime'' and repeat the same division in ''search()'' because then ''unsigned short'' is sufficient.
// Or compute ''largestPrime'' in a segmented way (it's similar to a segmented prime sieve anyway).
// Currently my solution of problem 407 is the "[most expensive](../performance/#slowandbig)": it consumes lots of memory and CPU power - but both are within Project Euler's limit.

#include <iostream>
#include <vector>

// find largest prime factor of each number up to "limit"
std::vector<unsigned int> largestPrime;

// fill "largestPrime"
void findLargestPrimes(unsigned int limit)
{
  largestPrime.resize(limit + 1, 1);
  for (unsigned long long i = 2; i <= limit; i++)
  {
    // skip if i is not a prime
    if (largestPrime[i] > 1)
      continue;

    // modify all multiples
    for (auto j = i; j <= limit; j += i)
      largestPrime[j] = i;

    // and all exponents i^2, i^3, i^4, ...
    auto j = i * i;
    while (j <= limit)
    {
      largestPrime[j] = j;
      j *= i;
    }
  }
}

// find maximum a that solves a^2 == a mod n
unsigned int search(unsigned int n)
{
  // don't forget about that special case n = 1 ...
  if (n == 1)
    return 0;

  // always return 1 if n has only one prime factor (is prime or perfect power of a prime, e.g. 32 = 2^5)
  auto p = largestPrime[n];
  if (p == n)
    return 1;

  // largest multiple of n's largest prime factor
  auto start = n - (n % p);
  // go backwards through all multiples of step
  for (unsigned long long x = start; x >= p; x -= p)
  {
    // check a - 1 = x which is a = x + 1
    auto next = x + 1;
    if ((next * next) % n == next)
      return next;

    // check a
    if ((x * x) % n == x)
      return x;
  }

  return 1;
}

// simple algorithm (pretty slow)
unsigned int slow(unsigned int n)
{
  unsigned long long a = n - 1;
  while (a >= 1)
  {
    // found a solution ?
    if ((a * a) % n == a)
      break;

    // no, keep going ...
    a--;
  }

  return a;
}

int main()
{
  unsigned int limit = 10000000;
  std::cin >> limit;

  // find largest prime factor of each number up to "limit"
  findLargestPrimes(limit);

  unsigned long long sum = 0;
  for (unsigned int n = 1; n <= limit; n++)
  {
    //auto current = slow(n);
    auto current = search(n);
    sum += current;
  }

  // display result
  std::cout << sum << std::endl;
  return 0;
}
