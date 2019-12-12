// ////////////////////////////////////////////////////////
// # Title
// (prime-k) factorial
//
// # URL
// https://projecteuler.net/problem=381
// http://euler.stephan-brumme.com/381/
//
// # Problem
// For a prime `p` let `S(p) = (sum{(p-k)!}) mod(p)` for `1 <= k <= 5`.
//
// For example, if `p=7`,
// `(7-1)! + (7-2)! + (7-3)! + (7-4)! + (7-5)! = 6! + 5! + 4! + 3! + 2! = 720+120+24+6+2 = 872`.
// As `872 mod(7) = 4`, `S(7) = 4`.
//
// It can be verified that `sum{S(p)} = 480` for `5 <= p < 100`.
//
// Find `sum{S(p)}` for `5 <= p < 10^8`.
//
// # Solved by
// Stephan Brumme
// August 2017
//
// # Algorithm
// The standard prime sieve from my [toolbox](/toolbox/) returns all primes up to `10^8` in about 0.2 seconds.
//
// Finding the factorials mod some value can be solved for small values with ''naive()'' - and as always, it's too slow.
// Browsing the Wikipedia I came across Wilson's theorem (see https://en.wikipedia.org/wiki/Wilson%27s_theorem ):
// `(n - 1)! == -1 (mod n)` iff `n` is a prime number
//
// Since `n - 1 == -1 (mod n)` I don't need negative numbers:
// `(n - 1)! == n - 1 (mod n)`
//
// Now that I can directly solve `(n - 1)!` I need a way to solve `(n - 2)!`, `(n - 3)!`, ...:
// `n! = (n - 1)! * n`
// `dfrac{n!}{n} = (n - 1)!`
//
// Unfortunately those equations only partially transfer from "normal" arithmetic to modular arithmetic because it's missing the division.
// But division can be emulated by multiplication:
// `dfrac{a}{a} = 1`
// `a * a^-1 = 1`
//
// In this case `a^-1` is the inverse of `a`. In modular arithmetic you can find the modular inverse `a^-1` of `a` as well with certain algorithms.
//
// My first approach was based on Fermat's Little Theorem (see https://en.wikipedia.org/wiki/Fermat%27s_little_theorem ):
// `a^p == a mod p`
// `a^(p-1) == 1 mod p`
// `a^(p-2) == a^-1 mod p`
// The left side is simply ''powmod(a, p - 2, p)''.
//
// My second approach turned out to be 4x faster (Extended Euclidean algorithm, see https://en.wikipedia.org/wiki/Extended_Euclidean_algorithm ):
// I solve `gcd(a,b) = ax + by` that means if `a` and `b` are coprime then `gcd(a,b) == 1` which means `1 = ax + by`:
// when I set `b=p` and apply the modulo to both sides:
// `1 mod p = (ax + py) mod p`
// `1 mod p == 1` and `py` is a multiple of `p`:
// `1 = ax mod p`
// ==> therefore `x` must be the modular inverse of `a mod p`
//
// The current version of my program can compute any factorial `(p - k)! mod p`.
// For performance reasons it only computes `(p - 5)!`. Then `(p - 4)! mod p = ((p - 4) * (p - 5)!) mod p`.
// After repeating the same for `p - 3` and `p - 2` I don't need to do it for `p - 1` because I already know that `(p - 1)! mod p == p - 1`.
// Adding those 5 terms is bigger than `p`, so I need to apply `mod p` once again.

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

// ---------- problem specific code ----------

// compute n! % modulo (slow and obvious algorithm)
unsigned int naive(unsigned int n, unsigned int modulo)
{
  unsigned long long result = 1;
  while (n > 1)
  {
    result *= n;
    result %= modulo;
    n--;
  }
  return (unsigned int)result;
}

// return (base^exponent) % modulo for 32-bit values, no need for mulmod
// copied from my toolbox, not needed anymore because replaced by the next function
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

// extended Euclidean algorithm, see also https://en.wikipedia.org/wiki/Extended_Euclidean_algorithm
// simplified version of ExtendedGcd from problem 134
unsigned int modularInverse(unsigned int a, unsigned int modulo)
{
  // pretty much the same code can be found on https://rosettacode.org/wiki/Modular_inverse

  auto originalModulo = modulo;
  // note: s and t can be negative inside the loop
  int s = 0;
  int t = 1;
  while (a > 1)
  {
    auto tmp      =     modulo;
    auto quotient = a / modulo;
    modulo        = a % modulo;
    a = tmp;

    auto tmp2 = s;
    s = t - quotient * s;
    t = tmp2;
  }

  // avoid negative result
  return t < 0 ? t + originalModulo : t;
}

// return n! % modulo (only if modulo is prime)
// https://en.wikipedia.org/wiki/Wilson%27s_theorem
unsigned int facmod(unsigned int n, unsigned int modulo)
{
  // if n >= modulo then n! is a multiple of modulo
  if (n >= modulo)
    return 0;

  // for small n the naive algorithm can be faster

  // Wilson's Theorem is: (n-1)! == -1 (mod modulo)
  // avoid negative numbers:  -1 == modulo - 1 (mod modulo)
  unsigned long long result = modulo - 1;

  // (p - i)! = p! / (p-1)   / (p-2)   / (p-3)    / ... / (p - i + 1)
  //          = p! * (p-1)^1 * (p-2)^1 * (p-3)^-1 * ... * (p - i + 1)^-1
  // with modular arithmetic the inverse of a is a^-1
  // where a^-1 can be found with Fermat's Little Theorem
  for (auto i = modulo - 1; i > n; i--)
  {
    // my first approach
    // Fermat's Little Theorem: a^p     == a mod p
    // divide both sides by a:  a^(p-1) == 1 mod p
    // once more:               a^(p-2) == a^-1 mod p
    // where a^-1 is the inverse of a => equal to a^(p-2) % p
    // (in my code: i=a and modulo=p)
    // https://en.wikipedia.org/wiki/Fermat%27s_little_theorem
    //result *= powmod(i, modulo - 2, modulo);
    //result %= modulo;

    // my second approach (4x faster):
    // Extended Euclidean algorithm https://en.wikipedia.org/wiki/Extended_Euclidean_algorithm
    // solve gcd(a,b) = ax + by
    // if a and b are coprime then gcd(a,b) == 1 which means:
    // 1 = ax + by
    // when I set b=p and apply the modulo to both sides:
    // 1 mod p = (ax + py) mod p
    // 1 mod p == 1 and py is a multiple of p:
    // 1 = ax mod p
    // therefore x must be the inverse of a modulo p
    result *= modularInverse(i, modulo);
    result %= modulo;
  }

  return (unsigned int)result;
}

int main()
{
  unsigned int limit = 100000000;
  std::cin >> limit;

  // generate prime numbers
  fillSieve(limit);

  // iterate over all prime numbers
  unsigned long long sum = 0;
  for (unsigned int i = 5; i < limit; i++)
    if (isPrime(i))
    {
      unsigned long long minus5 = facmod(i - 5, i);
      // facmod is "expensive", therefore re-use the result:
      // facmod(i - 4, i) = facmod(i - 5, i) * (i - 4) % i
      unsigned long long minus4 = (minus5 * (i - 4)) % i; // same as facmod(i - 4, i)
      // and the same trick for the remaining values
      unsigned long long minus3 = (minus4 * (i - 3)) % i; // same as facmod(i - 3, i)
      unsigned long long minus2 = (minus3 * (i - 2)) % i; // same as facmod(i - 2, i)
      unsigned long long minus1 = i - 1; // again Wilson's theorem

      sum += (minus1 + minus2 + minus3 + minus4 + minus5) % i;

      // note:
      // of course I could customize facmod() to emit the relevant values minus1, minus2, ..., minus5
      // (because they are generated internally) but I wanted it to keep the code simple in case I need it for other problems
    }

  // display result
  std::cout << sum << std::endl;
  return 0;
}
