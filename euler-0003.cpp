// ////////////////////////////////////////////////////////
// # Title
// Largest prime factor
//
// # URL
// https://projecteuler.net/problem=3
// http://euler.stephan-brumme.com/3/
//
// # Problem
// The prime factors of 13195 are 5, 7, 13 and 29.
// What is the largest prime factor of the number 600851475143 ?
//
// # Solved by
// Stephan Brumme
// February 2017
//
// # Algorithm
// Each composite number `x` can be represented as the product of at least two factors: `x=factor*other`
// If we assume that `factor` is a prime number and `factor<=other`, then there are two options:
// 1. `other` can be a prime number, too
// 2. `other` is composite
//
// In case 1, `other` is the largest prime - and we are done.
// In case 2, we can continue the same process by setting set `x=other`.
// After some iterations we will hit case 1.
//
// Therefore I start a loop beginning with `factor=2` (the smallest prime)
// and as long as our number `x` can be divided by `factor` with remainder 0:
// - divide `x` by `factor`, but abort if `x==factor` because then we have found our largest prime factor.
//
// We can abort as soon as all `factor<=sqrt{x}` are processed because then only a prime is left.
//
// # Note
// You may have noticed that `factor` isn't always a prime number in my program:
// yes, I simply scan through all numbers, even composite ones.
//
// But if they are composite, then I already checked all smaller primes.
// That means, I checked all prime factors of that composite number, too.
// Therefore `x` can't be divided by a composite `factor` with remainder 0 because
// all required prime factors were already eliminated from `x`.
//
// In short: those divisions by composite numbers always fail but my program is still fast enough and
// writing a proper prime sieve doesn't give a significant speed boost for this problem.

#include <iostream>

int main()
{
  unsigned int tests;
  std::cin >> tests;
  while (tests--)
  {
    unsigned long long x;
    std::cin >> x;

    // x can be represented by x=factor*otherFactor
    // where factor <= otherFactor
    // therefore factor <= sqrt(x)
    for (unsigned long long factor = 2; factor * factor <= x; factor++)
      // remove factor, actually it's a prime
      // (can occur multiple times, e.g. 20=2*2*5)
      while (x % factor == 0 && x != factor) // note: keep last prime
        x /= factor;

    std::cout << x << std::endl;
  }
  return 0;
}
