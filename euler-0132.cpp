// ////////////////////////////////////////////////////////
// # Title
// Large repunit factors
//
// # URL
// https://projecteuler.net/problem=132
// http://euler.stephan-brumme.com/132/
//
// # Problem
// A number consisting entirely of ones is called a repunit. We shall define `R(k)` to be a repunit of length `k`.
//
// For example, `R(10) = 1111111111 = 11 * 41 * 271 * 9091`, and the sum of these prime factors is 9414.
//
// Find the sum of the first forty prime factors of `R(10^9)`.
//
// # Solved by
// Stephan Brumme
// June 2017
//
// # Algorithm
// This problem was a tough one for me. I didn't realize that `R(k) = dfrac{10^k - 1}{9}` until I saw that equation on https://en.wikipedia.org/wiki/Repunit
//
// The task is to find prime factors `p` such that `R(k) == 0 mod p`.
// Which becomes (using the formula from Wikipedia):
// `dfrac{10^k - 1}{9} == 0 mod p`
//
// `10^k - 1 == 0 mod 9p`
// `10^k == 1 mod 9p`
//
// `k` is quite small, therefore a simplified version of my ''powmod'' function (see my [toolbox](../toolbox/#powmod) ) can easily compute ''10^k mod 9p''.
// If it returns 1, then ''p'' is added to ''sum'' until 40 factors are found.
//
// # Note
// My live tests allows you to enter different `k` and change the number of relevant prime factors, too (which is 40 in the original problem).

#include <iostream>
#include <vector>

// return (base^exponent) % modulo
unsigned int powmod(unsigned int base, unsigned int exponent, unsigned int modulo)
{
  unsigned int result = 1;
  while (exponent > 0)
  {
    // fast exponentation:
    // odd exponent ? a^b = a*a^(b-1)
    if (exponent & 1)
      result = (result * (unsigned long long)base) % modulo; // enforce 64-multiplication

    // even exponent ? a^b = (a*a)^(b/2)
    base = (base * (unsigned long long)base) % modulo;       // enforce 64-multiplication
    exponent >>= 1;
  }
  return result;
}

int main()
{
  unsigned int numFactors = 40;
  unsigned long long digits = 1000000000;
  std::cin >> digits >> numFactors;

  unsigned int sum = 0;
  std::vector<unsigned int> primes = { 2 }; // two is never a prime factor of a repunit
  for (unsigned int i = 3; numFactors > 0; i++)
  {
    bool isPrime = true;

    // test against all prime numbers we have so far (in ascending order)
    for (auto x : primes)
    {
      // prime is too large to be a divisor
      if (x*x > i)
        break;

      // divisible => not prime
      if (i % x == 0)
      {
        isPrime = false;
        break;
      }
    }

    // no prime
    if (!isPrime)
      continue;

    primes.push_back(i);

    // check for divisibility by 9*prime
    auto modulo = 9 * i;
    // remainder must be 1
    auto remainder = powmod(10, digits, modulo);
    if (remainder == 1)
    {
      sum += i;
      numFactors--;
    }

    // don't analyze huge primes
    // => not needed for the original problem, but avoids malicious input in live test
    // => e.g. user asks for more primes than the repunit has
    if (i > 1111111)
      break;
  }

  std::cout << sum << std::endl;

  return 0;
}
