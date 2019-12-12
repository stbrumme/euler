// ////////////////////////////////////////////////////////
// # Title
// Goldbach's other conjecture
//
// # URL
// https://projecteuler.net/problem=46
// http://euler.stephan-brumme.com/46/
//
// # Problem
// It was proposed by Christian Goldbach that every odd composite number can be written as the sum of a prime and twice a square.
//
// ` 9 =  7 + 2 * 1^2`
// `15 =  7 + 2 * 2^2`
// `21 =  3 + 2 * 3^2`
// `25 =  7 + 2 * 3^2`
// `27 = 19 + 2 * 2^2`
// `33 = 31 + 2 * 1^2`
//
// It turns out that the conjecture was false.
// What is the smallest odd composite that cannot be written as the sum of a prime and twice a square?
//
// # Solved by
// Stephan Brumme
// February 2017
//
// # Algorithm
// A standard prime sieve quickly finds all primes up to 500000.
// For all odd numbers `i` my program generate all squares `j^2 < i`.
// If no `j` exists such that `i - j^2` is prime then Goldbach's other conjecture is refuted.
//
// # Hackerrank
// Again, the Hackerrank problem is significantly different from the original Project Euler problem:
// we have to find all ways to represent it as a sum of a prime number and twice a square.
//
// My program generates all squares `j^2 < i` and count how often `i - j^2` is prime.

#include <set>
#include <iostream>

int main()
{
  const unsigned int MaxPrime = 500000;
  // find all primes up to 500000
  std::set<unsigned int> primes;
  primes.insert(2);
  for (unsigned int i = 3; i < MaxPrime; i += 2)
  {
    bool isPrime = true;
    // test against all prime numbers we have so far (in ascending order)
    for (auto p : primes)
    {
      // next prime is too large to be a divisor
      if (p*p > i)
          break;

      // divisible => not prime
      if (i % p == 0)
      {
        isPrime = false;
        break;
      }
    }

    // yes, we have a prime number
    if (isPrime)
      primes.insert(i);
  }

//#define ORIGINAL
#ifdef ORIGINAL
  // start at 9 (smallest odd number which is not prime)
  for (unsigned int i = 9; i <= MaxPrime; i += 2)
  {
    // only composite numbers
    if (primes.count(i) != 0)
      continue;

    bool refuteConjecture = true;
    // try all squares
    for (unsigned int j = 1; 2*j*j < i; j++)
    {
      auto check = i - 2*j*j;
      // found a combination, conjecture is still valid
      if (primes.count(check) != 0)
      {
        refuteConjecture = false;
        break;
      }
    }

    // conjecture refuted !
    if (refuteConjecture)
    {
      std::cout << i << std::endl;
      break;
    }
  }

#else

  unsigned int tests;
  std::cin >> tests;
  while (tests--)
  {
    unsigned int i;
    std::cin >> i;

    // try all squares
    unsigned int solutions = 0;
    for (unsigned int j = 1; 2*j*j < i; j++)
    {
      // check whether i - j^2 is prime
      unsigned int check = i - 2*j*j;
      // yes, found another combination
      if (primes.count(check) != 0)
        solutions++;
    }

    std::cout << solutions << std::endl;
  }
#endif

  return 0;
}
