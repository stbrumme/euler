// ////////////////////////////////////////////////////////
// # Title
// Hexagonal orchards
//
// # URL
// https://projecteuler.net/problem=351
// http://euler.stephan-brumme.com/351/
//
// # Problem
// A hexagonal orchard of order `n` is a triangular lattice made up of points within a regular hexagon with side `n`.
// The following is an example of a hexagonal orchard of order 5:
//
// ![orchard](p351_hexorchard.png)
//
// Highlighted in green are the points which are hidden from the center by a point closer to it.
// It can be seen that for a hexagonal orchard of order 5, 30 points are hidden from the center.
//
// Let `H(n)` be the number of points hidden from the center in a hexagonal orchard of order `n`.
//
// `H(5) = 30`. `H(10) = 138`. `H(1 000) = 1177848`.
//
// Find `H(100 000 000)`.
//
// # Solved by
// Stephan Brumme
// September 2017
//
// # Algorithm
// All six "triangles" of such a hexagon can be treated equal. I count only the points of the upper-right triangle and multiply the result by 6.
// The point in the centre doesn't belong to any triangle and is never hidden.
//
// All points can be thought of as fractions: the points on the right side of centre is 1/1.
// The next ring (actually: part of a ring, I'm only looking at one six-th of it) is 1/2 and 2/2, then 1/3, 2/3, 3/3, followed by 1/4, 2/4, 3/4 and 4/4.
// Finally the outer-most ring consists of 1/5, 2/5, 3/5, 4/5 and 5/5 for a total of 15 points.
// The total number is equivalent to the triangular number `T(n) = n(n+1)/2` (with `T(5) = 15`, see https://en.wikipedia.org/wiki/Triangular_number )
//
// The hidden points are 2/2, 3/3, 2/4, 4/4 and 5/5. Those are exactly the fractions that are not proper fractions:
// both numerator and denominator share a common factor such that `gcd(n,d) > 1`.
// The ''bruteForce()'' algorithm checks the `gcd()` of all fractions. It finds `H(1000)` within less than 0.04 seconds but becomes very slow for larger values (I assume it's `O(n^2 log n)`).
//
// However, I've seen similar problems before: counting how often a certain number if coprime to other numbers can be done with Euler's totient function (see https://en.wikipedia.org/wiki/Euler%27s_totient_function ).
// The guys at Project Euler seem to really like this function because of its namesake ...
//
// Anyway, the number of visible points in a (partial) ring `i` is `phi(i)`. Then the number of hidden points in that (partial) ring `i` is `i - phi(i)`.
// The number of hidden points in a full ring is therefore `6 * (i - phi(i))`.
// The solution for an orchard of order `n` is
// (1) `sum_{i=1..n} 6 * (i - phi(i))`
// (2) `6 * sum_{i=1..n} (i - phi(i))`
// (3) `6 * (sum_{i=1..n} i - sum_{i=1..n} phi(i))`
// (4) `6 * (n(n+1)/2 - sum_{i=1..n} phi(i))`
//
// All I need is an efficient way to compute `phi(i)` for all `i <= 10^8`; that's the totient summary function (see http://mathworld.wolfram.com/TotientSummatoryFunction.html ).
// If I compute all `phi(x)` at once with a sieve then I need more than 400 MByte RAM which is more than the inofficial limit of Project Euler
// (see ''sumPhi()'', it takes about 3.5 seconds to finish).
//
// So I rewrote that sieve to work in a segmented way (see ''sumPhiSliced()''): compute `phi(x)` for `0 < x < 10^6`, then `10^6 <= x < 2 * 10^6`, and so on.
// The memory consumption shrinks down to just 30 MByte and the program actually becomes a bit faster because of better cache locality (3.1 seconds, minus 0.4 seconds).
// Admittedly, the code isn't that nice to look at: I added my prime sieve from my toolbox and it takes quite some code to properly handle all edge cases.

#include <iostream>
#include <vector>

// ---------- gcd() can be found in my toolbox, as well as my fast prime sieve ----------

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

// ---------- problem-specific code ----------

// count all fractions i/j where i is coprime to j
unsigned long long bruteForce(unsigned int limit)
{
  unsigned long long hidden = 0;
  for (unsigned int i = 1; i <= limit; i++)
    for (unsigned int j = 1; j <= i; j++)
      if (gcd(i, j) != 1)
        hidden++;
  return hidden;
}

// needs almost half a GByte RAM
unsigned long long sumPhi(unsigned int limit)
{
  // similar to http://www.geeksforgeeks.org/eulers-totient-function-for-all-numbers-smaller-than-or-equal-to-n/
  // alternatives can be found here: https://codegolf.stackexchange.com/questions/26739/super-speedy-totient-function

  // the sum
  unsigned long long result = 1; // phi(1) = 1

  // allocate enough memory, fill with 1,2,3,4,...
  std::vector<unsigned int> phi(limit + 1);
  for (unsigned int i = 0; i < phi.size(); i++)
    phi[i] = i;

  // look for prime numbers
  for (unsigned int i = 2; i <= limit; i++)
  {
    result += phi[i];

    // composite number, already computed phi(i)
    if (phi[i] != i)
      continue;

    // phi(prime) = prime - 1
    phi[i]--; // was initially phi[i] = prime
    result--;

    // adjust all multiples
    for (auto j = 2 * i; j <= limit; j += i)
      phi[j] = (phi[j] / i) * (i - 1);
  }

  return result;
}

// adjustable memory consumption
unsigned long long sumPhiSliced(unsigned int limit, unsigned int segmentSize = 1000000)
{
  // the sum
  unsigned long long result = 1; // phi(1) = 1

  // find all primes
  fillSieve(limit);
  // store them
  std::vector<unsigned int> primes = { 2 };
  primes.reserve(limit * 6 / 100); // about 6 million primes < 10^8
  for (unsigned int i = 3; i <= limit; i += 2)
    if (isPrime(i))
      primes.push_back(i);
  // release memory
  sieve.clear();
  sieve.shrink_to_fit();

  // allocate memory for a single segment
  std::vector<unsigned int> phi(segmentSize);

  // start with phi(2)
  for (unsigned int from = 2; from <= limit; from += segmentSize)
  {
    auto to = from + segmentSize;
    if (to > limit)
      to = limit + 1;
    // actual size is always identical to segmentSize except for the last slice
    auto size = to - from;

    // fill with from, from+1, from+2, ...
    for (unsigned int i = 0; i < size; i++)
      phi[i] = from + i;

    // process all multiples of all primes in the current segments
    for (unsigned int p = 0; p < primes.size(); p++)
    {
      auto current = primes[p];

      // find smallest multiple in current slice
      auto minJ = 2 * current;
      if (minJ < from)
      {
        minJ = (from / current) * current;
        if (minJ < from)
          minJ += current;
      }

      // adjust all multiples
      for (auto j = minJ; j < to; j += current)
      {
        auto index = j - from;
        phi[index] = (phi[index] / current) * (current - 1);
      }

      // don't forget about the primes
      if (current >= from && current < to)
        phi[current - from]--;
    }

    // add all phi of the current slice
    for (unsigned int i = 0; i < size; i++)
      result += phi[i];
  }

  return result;
}

int main()
{
  unsigned int limit = 100000000; // 10^8
  std::cin >> limit;

  // slow brute-force
  //unsigned long long result = bruteForce(limit);

  // fast algorithm
  auto triangle = (unsigned long long)limit * (limit + 1) / 2;

#define SLICED
#ifdef  SLICED
  auto result = triangle - sumPhiSliced(limit);
#else
  auto result = triangle - sumPhi(limit);
#endif

  // the previous code analyzed only one sixth of the hexagon
  result *= 6;

  std::cout << result << std::endl;
  return 0;
}
