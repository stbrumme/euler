// ////////////////////////////////////////////////////////
// # Title
// Summation of primes
//
// # URL
// https://projecteuler.net/problem=10
// http://euler.stephan-brumme.com/10/
//
// # Problem
// The sum of the primes below 10 is 2 + 3 + 5 + 7 = 17.
// Find the sum of all the primes below two million.
//
// # Solved by
// Stephan Brumme
// February 2017
//
// # Algorithm
// The prime sieve is more or less unchanged from problem 7.
// Then I create a lookup table ''sums'' which contains for each prime number `p`
// the sum of all prime numbers `<=p`.
//
// The test cases may contain numbers which are not prime, too.
// Therefore I use ''upper_bound'' to find the smallest entry which `>=p`.
// And since we that entry "is one step too far", I go back to the previous entry and print it.
//
// # Hackerrank
// My 2-step design was heavily influenced by Hackerrank's large number of test cases:
// the "expensive" precomputation is done once and the test cases are computationally very "cheap".

#include <iostream>
#include <vector>
#include <map>

int main()
{
  // prime numbers beyond this are not relevant for the problem
  const unsigned int MaxPrime = 2000000;

  // precompute all relevant prime numbers
  std::vector<unsigned int> primes;
  // the only even prime
  primes.push_back(2);
  // now check all odd numbers for primality
  for (unsigned int i = 3; i <= MaxPrime; i += 2)
  {
    bool isPrime = true;
    for (auto p : primes)
    {
      // no larger prime factor possible ?
      if (p*p > i)
        break;

      // no prime number ?
      if (i % p == 0)
      {
        isPrime = false;
        break;
      }
    }

    // yes, we have a new prime
    if (isPrime)
      primes.push_back(i);
  }

  // prime numbers were found in ascending order,
  // let's add their value and store in a map such that
  // [prime number] => [sum of all prime numbers up to the current]
  // note: long long is required to avoid overflows
  std::map<unsigned int, unsigned long long> sums;
  unsigned long long sum = 0;
  for (auto p : primes)
  {
    sum += p;
    sums[p] = sum;
  }

  // the test cases are more or less "smart" lookups
  unsigned int tests;
  std::cin >> tests;
  while (tests--)
  {
    unsigned int x;
    std::cin >> x;

    // find the closest prime number which is bigger than the input
    auto i = sums.upper_bound(x);
    // go back to the closest prime number which is smaller than the input
    i--;

    // show the sum associated to that prime number
    std::cout << i->second << std::endl;
  }
  return 0;
}
