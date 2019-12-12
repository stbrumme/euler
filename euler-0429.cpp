// ////////////////////////////////////////////////////////
// # Title
// Sum of squares of unitary divisors
//
// # URL
// https://projecteuler.net/problem=429
// http://euler.stephan-brumme.com/429/
//
// # Problem
// A unitary divisor `d` of a number `n` is a divisor of `n` that has the property `gcd(d, n/d) = 1`.
// The unitary divisors of `4! = 24` are `1`, `3`, `8` and `24`.
// The sum of their squares is `1^2 + 3^2 + 8^2 + 24^2 = 650`.
//
// Let `S(n)` represent the sum of the squares of the unitary divisors of `n`. Thus `S(4!)=650`.
//
// Find `S(10^8!) mod (10^9+9)`.
//
// # Solved by
// Stephan Brumme
// September 2017
//
// # Algorithm
// The main insight regarding unitary divisors is that if I have a prime factorization of `n!` then any prime factor can only be found
// in either `d` or `n!/d` but not both because otherwise they would share the prime factor `p` and therefore `gcd(d, n!/d) >= p`.
//
// Mathworld has the formula including some explanations (see http://mathworld.wolfram.com/UnitaryDivisorFunction.html ):
// (1) `sigma^{*}_2(p^{a_1}_1, p^{a_2}_2, ...) = (1 + p^{2a_1}_1) * (1 + p^{2a_2}_2) * ...`
// The resulting products will be huge and I have to `mod 10^9 + 9` quite often.
// I copied the ''powmod()'' function form my toolbox instead of computing `p^{2a_i}_i mod (10^9+9)` in a loop.
//
// Now all I need is a fast prime factorization of `n!` ... and it is pretty easy when using Legendre's formula (see https://en.wikipedia.org/wiki/Legendre%27s_formula ):
// (2) `v_p(n!) = sum^{infinity}_{i=1} \lfloor dfrac{n}{p^i} \rfloor`
//
// In plain English: to figure how often a prime `p` can be found in `n!` I have to compute `dfrac{n}{p} + dfrac{n}{p^2} + dfrac{n}{p^3} + ...` where each division is an integer division (skip the remainder).
// For example `v_2(100!) = dfrac{100}{2} + dfrac{100}{4} + dfrac{100}{8} + dfrac{100}{16} + dfrac{100}{32} + dfrac{100}{64} = 50 + 25 + 12 + 6 + 3 + 1 = 97`
//
// That means `100!` is a multiple of `2^97`.
//
// The inner loop of ''solve'' starts with ''power = p'' and in each iteration multiplies it by ''p'' to get ''p*p'', then ''p*p*p'' and so on.
// A little trick to accelerate the process is to abort early when ''p'' is a large prime `2p > n` because then I know that `dfrac{limit}{p} = 1` (about 30% faster due to avoiding slow division).
//
// # Note
// About half of the time is spent in my prime sieve.
// I could have merged the prime sieve and ''solve()'' but this would mess up the code and I seriously doubt that a significant performance boost could be observed.

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

// ---------- powmod is part of my toolbox, too ----------

// return (base^exponent) % modulo for 32-bit values, no need for mulmod
unsigned int powmod(unsigned int base, unsigned int exponent, unsigned int modulo)
{
  unsigned int result = 1;
  while (exponent > 0)
  {
    // fast exponentation:
    // odd exponent ? a^b = a*a^(b-1)
    if (exponent & 1)
      result = (result * (unsigned long long)base) % modulo;

    // even exponent ? a^b = (a*a)^(b/2)
    base = (base * (unsigned long long)base) % modulo;
    exponent >>= 1;
  }
  return result;
}

// ---------- problem-specific code ----------

unsigned long long solve(unsigned int limit, unsigned int modulo)
{
  unsigned long long result = 1;

  // find all prime numbers
  fillSieve(limit);

  for (unsigned int p = 2; p <= limit; p++)
  {
    // prime factors only
    if (!isPrime(p))
      continue;

    // will be p^1, p^2, p^3, p^4, ...
    unsigned long long power = p;
    // count how often current prime appears in factorial(limit)
    unsigned int count = 0;
    while (power <= limit)
    {
      // fast check for large powers
      if (2*power > limit)
      {
        count++;
        break;
      } // note: this if-block can be removed and you still get the correct result - albeit slower

      // count multiples
      auto multiples = limit / power;
      count += multiples;
      power *= p;
    }

    // (p*p)^x = p^2x
    // => p*p can be 64 bits and powmod for 64 bits is more complex and much slower
    result *= 1 + powmod(p, 2*count, modulo);
    result %= modulo;
  }

  return result;
}

int main()
{
  const unsigned int Modulo = 1000000009;
  unsigned int limit = 100000000; // 10^8
  std::cin >> limit;

  std::cout << solve(limit, Modulo) << std::endl;
  return 0;
}
