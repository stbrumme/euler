// ////////////////////////////////////////////////////////
// # Title
// Repunit nonfactors
//
// # URL
// https://projecteuler.net/problem=133
// http://euler.stephan-brumme.com/133/
//
// # Problem
// A number consisting entirely of ones is called a repunit. We shall define `R(k)` to be a repunit of length `k`; for example, `R(6) = 111111`.
//
// Let us consider repunits of the form `R(10^n)`.
//
// Although `R(10)`, `R(100)`, or `R(1000)` are not divisible by 17, `R(10000)` is divisible by 17.
// Yet there is no value of `n` for which `R(10^n)` will divide by 19. In fact, it is remarkable that 11, 17, 41, and 73 are the only four primes below one-hundred that can be a factor of `R(10^n)`.
//
// Find the sum of all the primes below one-hundred thousand that will never be a factor of R(10n).
//
// # Solved by
// Stephan Brumme
// June 2017
//
// # Algorithm
// After solving problem 132 I played around with the code and just got lucky:
// I tried looping over several repunits but it turned out it is sufficient to test only one really, really large repunit.
// Unfortunately, I cannot give any satisfying reasoning. I saw some proves on other websites, though.
//
// In order to work with such "really, really large repunit" I had to swap my ''powmod'' code from problem 132 with code that can handle 64 bit values.
// It's part of my [toolbox](../toolbox/), too.

#include <iostream>
#include <vector>

// return (a*b) % modulo
unsigned long long mulmod(unsigned long long a, unsigned long long b, unsigned long long modulo)
{
  // (a * b) % modulo = (a % modulo) * (b % modulo) % modulo
  a %= modulo;
  b %= modulo;

  // fast path
  if (a <= 0xFFFFFFF && b <= 0xFFFFFFF)
    return (a * b) % modulo;

  // we might encounter overflows (slow path)
  // the number of loops depends on b, therefore try to minimize b
  if (b > a)
    std::swap(a, b);

  // bitwise multiplication
  unsigned long long result = 0;
  while (a > 0 && b > 0)
  {
    // b is odd ? a*b = a + a*(b-1)
    if (b & 1)
    {
      result += a;
      result %= modulo;
      // skip b-- because the bit-shift at the end will remove the lowest bit anyway
    }

    // b is even ? a*b = (2*a)*(b/2)
    a <<= 1;
    a  %= modulo;

    // next bit
    b >>= 1;
  }

  return result;
}

// return (base^exponent) % modulo
unsigned long long powmod(unsigned long long base, unsigned long long exponent, unsigned long long modulo)
{
  unsigned long long result = 1;
  while (exponent > 0)
  {
    // fast exponentation:
    // odd exponent ? a^b = a*a^(b-1)
    if (exponent & 1)
      result = mulmod(result, base, modulo);

    // even exponent ? a^b = (a*a)^(b/2)
    base = mulmod(base, base, modulo);
    exponent >>= 1;
  }
  return result;
}

int main()
{
  // a large exponent for powmod => 10^(10^19)
  const unsigned long long digits = 10000000000000000000ULL;

  unsigned int tests = 1;
  std::cin >> tests;
  while (tests--)
  {
    unsigned int maxPrime = 100000;
    std::cin >> maxPrime;

    unsigned long long sum = 0;
    std::vector<unsigned int> primes;
    for (unsigned int i = 2; i < maxPrime; i++)
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
      // remainder must not be 1
      auto remainder = powmod(10, digits, modulo);
      if (remainder != 1)
        sum += i;
    }

    std::cout << sum << std::endl;
  }

  return 0;
}
