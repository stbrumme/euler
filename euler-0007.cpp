// ////////////////////////////////////////////////////////
// # Title
// 10001st prime
//
// # URL
// https://projecteuler.net/problem=7
// http://euler.stephan-brumme.com/7/
//
// # Problem
// By listing the first six prime numbers: 2, 3, 5, 7, 11, and 13, we can see that the 6th prime is 13.
// What is the 10001st prime number?
//
// # Solved by
// Stephan Brumme
// February 2017
//
// # Algorithm
// A prime number is an integer number `p>=2` that can only be divided by `1` and by itself (`p`).
// 2 is the smallest prime number and the only even prime number, too (all other prime numbers are odd).
//
// Each number `x` can be split into its prime factors, that means we check for all primes `p<x` whether `x mod p == 0`.
// If that test fails for all those primes, then `x` is a prime number and can be added to our ''std::vector''.
//
// # Note
// Actually we can abort the loop if `p>=sqrt{x}` (which is `p^2>=x`) to speed up the program.
//
// And since all primes are odd - __except for 2__ - I simply add 2 to the list of primes and then scan
// only odd numbers, beginning with 3 (and increment 2).
//
// # Alternative
// Take a look at my [toolbox](../toolbox/) for other prime sieves or even precomputed lookup tables.
//
// Wikipedia lists a few faster algorithms ( https://en.wikipedia.org/wiki/Prime_number ), too.
// On my website http://create.stephan-brumme.com/eratosthenes/ you can find parallelized code that computes
// all 50847534 prime numbers below 1 billion in less than a second.
//
// # I/O
// echo "1 10001" | ./euler-0007

#include <iostream>
#include <vector>

int main()
{
  // compute the first 10001 primes
  std::vector<unsigned int> primes;
  primes.reserve(10001);
  primes.push_back(2);
  for (unsigned int x = 3; primes.size() <= 10000; x += 2)
  {
    bool isPrime = true;
    for (auto p : primes)
    {
      // found a divisor ? => abort
      if (x % p == 0)
      {
        isPrime = false;
        break;
      }

      // no larger prime factors possible ?
      if (p*p > x)
        break;
    }

    // yes, we have a new prime
    if (isPrime)
      primes.push_back(x);
  }

  // processing all test cases is now just a plain lookup
  unsigned int tests;
  std::cin >> tests;
  while (tests--)
  {
      unsigned int x;
      std::cin >> x;
      // just look up the x-th prime
      // with a little twist: vector's index is zero-based, therefore "off by one"
      x--;

      if (x < primes.size())
        std::cout << primes[x] << std::endl;
      else
        std::cout << "ERROR" << std::endl;
  }
  return 0;
}
