// ////////////////////////////////////////////////////////
// # Title
// Prime summations
//
// # URL
// https://projecteuler.net/problem=77
// http://euler.stephan-brumme.com/77/
//
// # Problem
// It is possible to write ten as the sum of primes in exactly five different ways:
//
// 7 + 3
// 5 + 5
// 5 + 3 + 2
// 3 + 3 + 2 + 2
// 2 + 2 + 2 + 2 + 2
//
// What is the first value which can be written as the sum of primes in over five thousand different ways?
//
// # Solved by
// Stephan Brumme
// March 2017
//
// # Algorithm
// Instead of re-using the code from problem 76 (which was based on probem 31) I wrote this code from scratch
// because the solution is actually much simpler than the previous challenges.
//
// Main idea:
// ''combinations(x) = combinations(x - prime1) + combinations(x - prime2) + ...''
// I subtract each prime and look up ''combinations(x - currentPrime)'' and sum all those numbers
// ==> nice Dynamic Programming solution !

#include <iostream>
#include <vector>

int main()
{
  const unsigned int MaxNumber = 1000;
  // store number of ways to represent a number as a sum of primes
  std::vector<unsigned long long> combinations(MaxNumber + 1, 0);
  // degenerated case
  combinations[0] = 1;

  // store all primes
  std::vector<unsigned int> primes;
  for (unsigned int i = 2; i <= MaxNumber; i++)
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

    // only primes after this point ...
    if (!isPrime)
      continue;

    primes.push_back(i);

    // now add all solutions
    for (unsigned int pos = 0; pos <= MaxNumber - i; pos++)
      combinations[pos + i] += combinations[pos];
  }

//#define ORIGINAL
#ifdef ORIGINAL
  // find first number with more than 5000 combinations
  for (size_t i = 0; i < combinations.size(); i++)
    if (combinations[i] > 5000)
    {
      std::cout << i << std::endl;
      break;
    }
#else
  unsigned int tests = 1;
  std::cin >> tests;
  while (tests--)
  {
    // look up combinations
    unsigned int n;
    std::cin >> n;
    std::cout << combinations[n] << std::endl;
  }
#endif

  return 0;
}
