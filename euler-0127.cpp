// ////////////////////////////////////////////////////////
// # Title
// abc-hits
//
// # URL
// https://projecteuler.net/problem=127
// http://euler.stephan-brumme.com/127/
//
// # Problem
// The radical of `n`, `rad(n)`, is the product of distinct prime factors of `n`.
// For example, `504 = 23 * 32 * 7`, so `rad(504) = 2 * 3 * 7 = 42`.
//
// We shall define the triplet of positive integers `(a, b, c)` to be an abc-hit if:
//
// 1. `GCD(a, b) = GCD(a, c) = GCD(b, c) = 1`
// 2. `a < b`
// 3. `a + b = c`
// 4. `rad(abc) < c`
//
// For example, `(5, 27, 32)` is an abc-hit, because:
//
// 1. `GCD(5, 27) = GCD(5, 32) = GCD(27, 32) = 1`
// 2. `5 < 27`
// 3. `5 + 27 = 32`
// 4. `rad(4320) = 30 < 32`
//
// It turns out that abc-hits are quite rare and there are only thirty-one abc-hits for `c < 1000`, with `sum{c} = 12523`.
//
// Find `sum{c}` for `c < 120000`.
//
// # Solved by
// Stephan Brumme
// June 2017
//
// # Algorithm
// My first step is to produce all ''rad[]'' in a sieve-like manner.
// Then two nested loops process each pair `(a,b)` where `a < b` and `a + b < 120000`.
//
// Check whether ''rad[a*b*c] < c'':
// The problem statement clearly states that `gcd(a,b)=gcd(b,c)=gcd(a,c)=1`. If a, b and c have no shared divisors, then their rad have no shared divisors, too.
// It follows that ''rad[a*b*c] = rad[a] * rad[b] * rad[c]''.
//
// If `gcd(a,b)=1` then `gcd(a,c)=gcd(a,b+a)=1` and `gcd(b,c)=gcd(c,b)=gcd(a+b,b)=1`. It's sufficient to test only ''gcd(a,b) == 1''.
//
// A minor speedup comes from the observation that `a` and `b` can't be even at the same time because then they would both contain prime factor 2:
// if `a` is even then ''incrementB'' ensures that `b` is always odd.
//
// A second trick is that `gcd(a, b) != 1` only if `a` and `b` share at least one prime factor. However, all prime factors can be found in `rad(a)` and `rad(b)`, too.
// Therefore if `gcd(rad(a), rad(b)) = 1` then `gcd(a, b) = 1`. That's helpful since the iterative ''gcd()'' algorithm is faster with smaller parameters and `rad(x) <= x`.

#include <iostream>
#include <vector>
#include <cmath>

// greatest common divisor
unsigned int gcd(unsigned int a, unsigned int b)
{
  while (a != 0)
  {
    unsigned int c = a;
    a = b % a;
    b = c;
  }
  return b;
}

int main()
{
  unsigned int limit  = 120000;
  std::cin >> limit;

  // sieve-like computation of rad
  std::vector<unsigned int> rad(limit, 1);
  for (unsigned int i = 2; i < limit; i++)
  {
    // if rad(i) > 1 then "a" is not prime
    if (rad[i] > 1)
      continue;

    // process all multiples of a
    for (unsigned int j = 1; i*j < limit; j++)
      rad[i*j] *= i;
  }

  // walk through all potential a and b
  unsigned long long sum = 0;
  for (unsigned int a = 1; a < limit/2; a++)
  {
    unsigned int incrementB = 1;
    // make sure that if a is even then b is always odd
    if (a % 2 == 0)
      incrementB = 2;

    for (unsigned int b = a + 1; a + b < limit; b += incrementB)
    {
      auto c = a + b;

      // this check is much faster than the gcd()-computation and eliminates most candidates
      auto prodRad = rad[a] * rad[b] * (unsigned long long)rad[c];
      if (prodRad >= c)
        continue;

      // no shared prime factors
      if (gcd(rad[a], rad[b]) != 1)
        continue;

      // yes, found another abc-hit
      sum += c;
    }
  }

  // display result
  std::cout << sum << std::endl;

  return 0;
}
