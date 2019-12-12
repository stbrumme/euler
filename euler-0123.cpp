// ////////////////////////////////////////////////////////
// # Title
// Prime square remainders
//
// # URL
// https://projecteuler.net/problem=123
// http://euler.stephan-brumme.com/123/
//
// # Problem
// Let `p_n` be the `n`th prime: 2, 3, 5, 7, 11, ..., and let `r` be the remainder when `(p_n-1)^n + (p_n+1)^n` is divided by `pn^2`.
//
// For example, when `n = 3`, `p^3 = 5`, and `4^3 + 6^3 = 280 == 5 mod 25`.
//
// The least value of n for which the remainder first exceeds `10^9` is 7037.
//
// Find the least value of `n` for which the remainder first exceeds `10^10`.
//
// # Solved by
// Stephan Brumme
// May 2017
//
// # Algorithm
// This is basically problem 120 merged with a prime sieve.
// My algorithm description of problem 120 explains why the maximum remainder is `2na` (and `a` is a prime in the current problem).
//
// # Note
// There is no need to check the first 7036 prime numbers because the problem statement mentioned that all of them produce a remainder `<10^9`.
// But the computation is so simple and fast that it doesn't affect the execution time at all.
//
// I prefer a prime sieve based on trial division because it can be computed incrementally without knowing an upper limit beforehand
// (which is a must for the sieve of Eratosthenes).

#include <iostream>
#include <vector>

int main()
{
  // sieve based on trial division
  std::vector<unsigned int> primes = { 2 };
  for (unsigned int i = 3; ; i += 2)
  {
    bool isPrime = true;

    // test against all prime numbers we have so far (in ascending order)
    for (auto p : primes)
    {
      // next prime is too large to be a divisor ?
      if (p*p > i)
        break;

      // divisible ? => not prime
      if (i % p == 0)
      {
        isPrime = false;
        break;
      }
    }

    // no prime ?
    if (!isPrime)
      continue;

    primes.push_back(i);

    // only every "odd" prime
    if (primes.size() % 2 == 0)
      continue;

    // compute remainder
    auto remainder = 2 * i * primes.size();
    // exceeds 10^10 ?
    if (remainder >= 10000000000ULL)
      break;
  }

  // print result
  std::cout << primes.size() << std::endl;
  return 0;
}
