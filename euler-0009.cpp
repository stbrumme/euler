// ////////////////////////////////////////////////////////
// # Title
// Special Pythagorean triplet
//
// # URL
// https://projecteuler.net/problem=9
// http://euler.stephan-brumme.com/9/
//
// # Problem
// A Pythagorean triplet is a set of three natural numbers, `a < b < c`,
// for which, `a^2 + b^2 = c^2`
//
// For example, `3^2 + 4^2 = 9 + 16 = 25 = 5^2`.
//
// There exists exactly one Pythagorean triplet for which `a + b + c = 1000`.
// Find the product `abc`.
//
// # Solved by
// Stephan Brumme
// February 2017
//
// # Algorithm
// I loop through all pairs `a<b` and compute `c=sqrt{a^2+b^2}`.
// If `c` is an integer and `a+b+c<=3000` then the largest product `abc` is stored.
//
// # Hackerrank
// For some sums `a+b+c` multiple solutions might exist and the largest product `abc` should be returned.
// It is necessary to have a pre-computation step of all perimeters' solutions to handle the huge amount of test cases.

#include <iostream>
#include <vector>
#include <cmath>

int main()
{
  // precompute all pairs a<b<c where a+b+c <= 3000
  const int MaxPerimeter = 3000;
  // -1 means "no triplets" for that perimeter
  const int NoSolution   =   -1;

  // cache[0] remains unused
  std::vector<int> cache(MaxPerimeter + 1, NoSolution);

  // scan all pairs a<b
  for (int a = 1; a < MaxPerimeter; a++)
    for (int b = a + 1; b < MaxPerimeter - a; b++)
    {
      // find c
      int c2 = a*a + b*b;
      // approximate square root as integer
      int c = sqrt(c2);
      // was it the correct square root ?
      if (c*c != c2)
        continue;

      // check summing condition
      int sum = a + b + c;
      if (sum > MaxPerimeter)
        break;

      // better solution than before ?
      if (cache[sum] < a*b*c)
        cache[sum] = a*b*c;
    }

  unsigned int tests;
  std::cin >> tests;
  while (tests--)
  {
    unsigned int n;
    std::cin >> n;
    // just lookup results (-1 if no solution)
    std::cout << cache[n] << std::endl;
  }
  return 0;
}
