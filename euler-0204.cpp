// ////////////////////////////////////////////////////////
// # Title
// Generalised Hamming Numbers
//
// # URL
// https://projecteuler.net/problem=204
// http://euler.stephan-brumme.com/204/
//
// # Problem
// A Hamming number is a positive number which has no prime factor larger than 5.
// So the first few Hamming numbers are 1, 2, 3, 4, 5, 6, 8, 9, 10, 12, 15.
// There are 1105 Hamming numbers not exceeding `10^8`.
//
// We will call a positive number a generalised Hamming number of type `n`, if it has no prime factor larger than `n`.
// Hence the Hamming numbers are the generalised Hamming numbers of type 5.
//
// How many generalised Hamming numbers of type 100 are there which don't exceed `10^9`?
//
// # Solved by
// Stephan Brumme
// May 2017
//
// # Algorithm
// After all prime numbers between 2 and ''hamming'' are found, the function ''search'' builds all combinations of prime factors.

#include <iostream>
#include <vector>

// user-defined limits
unsigned int limit = 1000000000;
unsigned int hamming = 100;

// short list of involved primes
std::vector<unsigned int> primes;

// return number of hamming numbers where one prime factor is at least primes[indexMinPrime]
// and which is a multiple of x
unsigned int search(unsigned long long x = 1, unsigned int indexMinPrime = 0)
{
  unsigned int result = 1;

  // multiply current number by all allowed primes
  for (auto i = indexMinPrime; i < primes.size(); i++)
  {
    auto product = primes[i] * x;

    // too large ?
    if (product > limit)
      break;

    // multiply by more primes
    result += search(product, i);
  }

  return result;
}

int main()
{
  std::cin >> hamming >> limit;

  // the usual prime sieve
  for (unsigned int i = 2; i <= hamming; i++)
  {
    bool isPrime = true;
    for (auto p : primes)
    {
      if (p*p > i)
        break;
      if (i % p == 0)
      {
        isPrime = false;
        break;
      }
    }

    if (isPrime)
      primes.push_back(i);
  }

  // count hamming numbers
  std::cout << search() << std::endl;
}
