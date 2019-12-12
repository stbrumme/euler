// ////////////////////////////////////////////////////////
// # Title
// Pandigital prime
//
// # URL
// https://projecteuler.net/problem=41
// http://euler.stephan-brumme.com/41/
//
// # Problem
// We shall say that an n-digit number is pandigital if it makes use of all the digits 1 to n exactly once.
// For example, 2143 is a 4-digit pandigital and is also prime.
//
// What is the largest n-digit pandigital prime that exists?
//
// # Solved by
// Stephan Brumme
// February 2017
//
// # Algorithm
// The largest pandigital number is 987654321. In order to find out whether a number `x <= 987654321` is prime, I precompute all primes up to `sqrt{987654321} approx 31426`.
// These "small" prime numbers will be kept in ''smallPrimes''.
//
// My second step is to generate all pandigital numbers: I create all permutations of the string ''"123456789"'' and perform a simple primality test (using ''smallPrimes'').
// Due to Hackerrank's variable number of digits, not only the 9-pandigitals numbers but also the 8-, 7-, 6-, ..., 2-pandigital numbers are checked, too.
//
// The set ''panPrimes'' will contain all 2-, ..., 9-pandigital primes after those two precomputation steps.
// Each test case look ups the closest bigger pandigital prime (''upper_bound'') and goes one step backwards.
//
// # Note
// When looking at the results I only saw 4- and 7-pandigital primes.
// Modifying my loop in step 2 accordingly would provide a 10x speed-up.

#include <set>
#include <iostream>
#include <algorithm>

int main()
{
  // precomputation step 1:
  // find all primes below sqrt(987654321)
  std::set<unsigned int> smallPrimes;
  smallPrimes.insert(2);
  for (unsigned int i = 3; i*i <= 987654321; i += 2)
  {
    bool isPrime = true;
    for (auto p : smallPrimes)
    {
      // abort, no divisors possible
      if (p*p > i)
        break;

      // divisor found ?
      if (i % p == 0)
      {
        isPrime = false;
        break;
      }
    }

    // found a prime number
    if (isPrime)
      smallPrimes.insert(i);
  }

  // precomputation step 2:
  // generate all permutations of the strings "12", "123", "1234", ..., "123456789"
  // and test whether they are prime
  std::set<unsigned int> panPrimes;
  for (unsigned int digits = 2; digits <= 9; digits++)
  {
    std::string strNumber = "123456789";
    // reduce number of digits
    strNumber.erase(digits);

    do
    {
      unsigned int number = std::stoi(strNumber);

      // test whether pandigital number is prime
      bool isPrime = true;
      for (auto p : smallPrimes)
      {
        // abort, no divisors possible
        if (p*p > number)
          break;

        // divisor found ?
        if (number % p == 0)
        {
          isPrime = false;
          break;
        }
      }

      // found a pandigital prime ?
      if (isPrime)
        panPrimes.insert(number);
    } while (std::next_permutation(strNumber.begin(), strNumber.end()));
  }

  // process input
  unsigned int tests;
  std::cin >> tests;
  while (tests--)
  {
    unsigned int limit;
    std::cin >> limit;

    // find next larger pandigital prime number
    auto i = panPrimes.upper_bound(limit);

    // smaller than the smallest pandigital prime ?
    if (i == panPrimes.begin())
    {
      std::cout << "-1" << std::endl;
      continue;
    }

    // upper_bound() goes one step too far
    i--;
    // and print it
    std::cout << *i << std::endl;
  }

  return 0;
}
