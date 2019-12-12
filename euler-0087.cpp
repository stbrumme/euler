// ////////////////////////////////////////////////////////
// # Title
// Prime power triples
//
// # URL
// https://projecteuler.net/problem=87
// http://euler.stephan-brumme.com/87/
//
// # Problem
// The smallest number expressible as the sum of a prime square, prime cube, and prime fourth power is 28.
// In fact, there are exactly four numbers below fifty that can be expressed in such a way:
//
// `28 = 2^2 + 2^3 + 2^4`
// `33 = 3^2 + 2^3 + 2^4`
// `49 = 5^2 + 2^3 + 2^4`
// `47 = 2^2 + 3^3 + 2^4`
//
// How many numbers below fifty million can be expressed as the sum of a prime square, prime cube, and prime fourth power?
//
// # Solved by
// Stephan Brumme
// March 2017
//
// # Algorithm
// A simple prime sieve is responsible to find all primes `i < sqrt{50000000}`.
// Then three nested loops compute all sums of all combinations of such primes where `a^2 + b^3 + c^4 < 50000000`.
// Be careful: ''b*b*b'' and ''c*c*c*c'' can easily exceed an ''unsigned int''.
//
// Those sums are sorted and duplicate sums are removed. Now we have a nice ''std::vector'' with all sums.
// The original problem is solved now (just display ''sums.size()'') but the Hackerrank problem is slightly tougher.
//
// # Hackerrank
// In order to find the the number of sums which are below a certain input value, I search through the sorted container with ''std::upper_bound''
// and then compute the distance to the beginning of the container.
// That's extremely fast (''std::upper_bound'' most likely uses binary search) and easily processed thousands of test cases per second.

#include <vector>
#include <algorithm>
#include <iostream>

int main()
{
  const unsigned int MaxLimit = 100 * 1000 * 1000; // Hackerrank: 10^7 instead of 5*10^6

  // prime sieve
  std::vector<unsigned int> primes;
  primes.push_back(2);
  for (unsigned int i = 3; i*i < MaxLimit; i += 2)
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

    // yes, we have a prime number
    if (isPrime)
      primes.push_back(i);
  }

  // just three nested loops where I generate all sums
  std::vector<unsigned int> sums;
  for (auto a : primes)
    for (auto b : primes)
      for (auto c : primes)
      {
        auto a2 = a*a;
        auto b3 = (unsigned long long)b*b*b;
        auto c4 = (unsigned long long)c*c*c*c;
        auto sum = a2 + b3 + c4;
        // abort if too big
        if (sum > MaxLimit)
          break;

        sums.push_back(sum);
      }

  // sort ascendingly
  std::sort(sums.begin(), sums.end());
  // a few sums occur twice, let's remove them !
  auto last = std::unique(sums.begin(), sums.end());

  // process test cases
  unsigned int tests = 1;
  std::cin >> tests;
  while (tests--)
  {
    unsigned int limit = MaxLimit;
    std::cin >> limit;

    // find next sum which is bigger than the limit
    auto pos = std::upper_bound(sums.begin(), last, limit);
    // how many sums are inbetween 28 and limit ?
    auto num = std::distance(sums.begin(), pos);
    std::cout << num << std::endl;
  }

  return 0;
}
