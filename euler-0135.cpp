// ////////////////////////////////////////////////////////
// # Title
// Same differences
//
// # URL
// https://projecteuler.net/problem=135
// http://euler.stephan-brumme.com/135/
//
// # Problem
// Given the positive integers, `x`, `y`, and `z`, are consecutive terms of an arithmetic progression, the least value of the positive integer, `n`,
// for which the equation, `x^2 - y^2 - z^2 = n`, has exactly two solutions is `n = 27`:
//
// `34^2 - 27^2 - 20^2 = 12^2 - 9^2 - 6^2 = 27`
//
// It turns out that n = 1155 is the least value which has exactly ten solutions.
//
// How many values of `n` less than one million have exactly ten distinct solutions?
//
// # Solved by
// Stephan Brumme
// May 2017
//
// # Algorithm
// Let's assume `y = a`, `x = a + b` and `z = a - b`. Then I have to solve:
// `(a + b)^2 - a^2 - (a - b)^2`
// `= a^2 + 2ab + b^2 - a^2 - a^2 + 2ab - b^2`
// `= 4ab - a^2`
// `= a(4b - a)`
//
// All variables are positive integers, therefore `1 <= a < n`.
// The value inside the brackets has to be positive, too, and `b` must be lower than `a` such that `\lceil frac{a}{4} \rceil <= b < a`
//
// Finally, iterating over all ''solutions'' and counting how often 10 occurs given the desired result.
//
// # Hackerrank
// Just print how many solutions exists for a given input. The upper limit is 8 million (inclusive) opposed to one million (exclusive) of the original problem.

#include <iostream>
#include <vector>

//#define ORIGINAL

int main()
{
#ifdef ORIGINAL
  unsigned int limit = 1000000; // "less than one million"
#else
  unsigned int limit = 8000001; // up to 8 million (inclusive)
#endif

  // precompute solutions
  std::vector<unsigned int> solutions(limit, 0);
  for (unsigned int a = 1; a < limit; a++)
    for (auto b = (a + 3) / 4; b < a; b++)
    {
      auto current = a * (4*b - a);
      if (current >= limit)
        break;

      solutions[current]++;
    }

#ifdef ORIGINAL
  // count all with exactly 10 solutions
  unsigned int count = 0;
  for (auto s : solutions)
    if (s == 10)
      count++;
  std::cout << count << std::endl;

#else

  // look up number of solutions
  unsigned int tests;
  std::cin >> tests;
  while (tests--)
  {
    unsigned int pos;
    std::cin >> pos;
    std::cout << solutions[pos] << std::endl;
  }
#endif

  return 0;
}
