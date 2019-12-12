// ////////////////////////////////////////////////////////
// # Title
// Quadratic primes
//
// # URL
// https://projecteuler.net/problem=27
// http://euler.stephan-brumme.com/27/
//
// # Problem
// Euler discovered the remarkable quadratic formula: `n^2+n+41`
//
// It turns out that the formula will produce 40 primes for the consecutive integer values `0<=n<=39`.
// However, when `n=40`, `40^2+40+41=40(40+1)+41` is divisible by 41, and certainly when `n=41`, `41^2+41+41` is clearly divisible by 41.
//
// The incredible formula `n^2-79n+1601` was discovered, which produces 80 primes for the consecutive values `0<=n<=79`.
// The product of the coefficients, `-79` and `1601`, is `-126479`.
//
// Considering quadratics of the form:
// `n^2 + a * n + b`, where `|a|<1000` and `|b|<=1000` where `|n|` is the modulus/absolute value of `n` e.g. `|11|=11` and `|-4|=4`
//
// Find the product of the coefficients, `a` and `b`, for the quadratic expression that produces the maximum number of primes for consecutive values of `n`, starting with `n=0`.
//
// # Solved by
// Stephan Brumme
// February 2017
//
// # Algorithm
// Nothing fancy: ''isPrime'' determines whether its parameter is a prime number or not.
// And then two nested loops check every combination of ''a'' and ''b''.
//
// # Note
// ''isPrime'' can be optimized in various ways - but the basic algorithm is fast enough for the problem.

#include <iostream>

// return true if x is prime
bool isPrime(int x)
{
  // reject invalid input
  if (x <= 1)
    return false;

  // process all potential divisors
  for (int factor = 2; factor*factor <= x; factor++)
    if (x % factor == 0)
      return false;

  // no such divisor found, it's a prime number
  return true;
}

int main()
{
  // upper and lower limit of the coefficients
  int limit;
  std::cin >> limit;
  // make sure it's a positive number
  if (limit < 0)
    limit = -limit;

  // keep track of best sequence:
  // number of generated primes
  unsigned int consecutive = 0;
  // its coefficients
  int bestA = 0;
  int bestB = 0;

  // simple brute-force approach
  for (int a = -limit; a <= +limit; a++)
    for (int b = -limit; b <= +limit; b++)
    {
      // count number of consecutive prime numbers
      unsigned int length = 0;
      while (isPrime(length * length + a * length + b))
        length++;

      // better than before ?
      if (consecutive < length)
      {
        consecutive = length;
        bestA = a;
        bestB = b;
      }
    }

#define ORIGINAL
#ifdef ORIGINAL
  // print a*b
  std::cout << (bestA * bestB) << std::endl;
#else
  // print best factors
  std::cout << bestA << " " << bestB << std::endl;
#endif
  return 0;
}
