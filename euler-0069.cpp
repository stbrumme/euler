// ////////////////////////////////////////////////////////
// # Title
// Totient maximum
//
// # URL
// https://projecteuler.net/problem=69
// http://euler.stephan-brumme.com/69/
//
// # Problem
// Euler's Totient function, `phi(n)` [sometimes called the phi function], is used to determine the number of numbers less than `n` which are relatively prime to `n`.
// For example, as 1, 2, 4, 5, 7, and 8, are all less than nine and relatively prime to nine, `phi(9)=6`.
//
// ||  2  ||          8        ||    4     ||     4      ||
// || `n` || Relatively Prime  || `phi(n)` || `n/phi(n)` ||
// ||-----||-------------------||----------||------------||
// ||  2  || 1                 ||    1     || 2          ||
// ||  3  || 1,2               ||    2     || 1.5        ||
// ||  4  || 1,3               ||    2     || 2          ||
// ||  5  || 1,2,3,4           ||    4     || 1.25       ||
// ||  6  || 1,5               ||    2     || 3          ||
// ||  7  || 1,2,3,4,5,6       ||    6     || 1.1666...  ||
// ||  8  || 1,3,5,7           ||    4     || 2          ||
// ||  9  || 1,2,4,5,7,8       ||    6     || 1.5        ||
// || 10  || 1,3,7,9           ||    4     || 2.5        ||
//
// It can be seen that `n=6` produces a maximum `n/phi(n)` for `n <= 10`.
//
// Find the value of `n <= 1000000` for which `n/phi(n)` is a maximum.
//
// # Solved by
// Stephan Brumme
// March 2017
//
// # Algorithm
// I have no formal proof yet (it's too late - time to go to bed !):
// in my first brute-force attempt I observed that the "best number" is the product of all primes
// `best = 2 * 3 * 5 * 7 * 11 * 13 * ...` where `best < 1000000`.
//
// Simple tests showed that all primes from 2 to 57 are sufficient.
//
// # Hackerrank
// The test ''best * nextPrime >= limit'' might overflow.
// The same result can be achieved this way:
// `best * nextPrime >= limit`
//
// `best             >= dfrac{limit}{nextPrime}`
//
// All variables are integers and thus rounding comes into play.
// The correct formula is:
// `best             >= dfrac{limit}{nextPrime} + dfrac{nextPrime - 1}{nextPrime}`
//
// `best             >= dfrac{limit + nextPrime - 1}{nextPrime}`

#include <iostream>

int main()
{
  // enough primes for this problem
  const unsigned int primes[] = { 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 57 };

  unsigned int tests;
  std::cin >> tests;
  while (tests--)
  {
    unsigned long long limit;
    std::cin >> limit;

    // multiply until we hit our limit
    unsigned long long bestPos = 1;
    for (auto p : primes)
    {
      // continue until bestPos reaches or exceeds our input value
      //__int128 next = bestPos * p;
      //if (next >= limit)
      //  break;
      // same code as before but more portable:
      if (bestPos >= (limit + p - 1) / p)
        break;

      bestPos *= p;
    }

    std::cout << bestPos << std::endl;
  }
  return 0;
}
