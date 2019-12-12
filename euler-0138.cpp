// ////////////////////////////////////////////////////////
// # Title
// Special isosceles triangles
//
// # URL
// https://projecteuler.net/problem=138
// http://euler.stephan-brumme.com/138/
//
// # Problem
// Consider the isosceles triangle with base length, `b = 16`, and legs, `L = 17`.
//
// ![triangle](p138.gif)
//
// By using the Pythagorean theorem it can be seen that the height of the triangle, `h = sqrt{172 - 82} = 15`, which is one less than the base length.
//
// With `b = 272` and `L = 305`, we get `h = 273`, which is one more than the base length, and this is the second smallest isosceles triangle with the property that `h = b \pm 1`.
//
// Find `sum{L}` for the twelve smallest isosceles triangles for which `h = b \pm 1` and `b`, `L` are positive integers.
//
// # Solved by
// Stephan Brumme
// July 2017
//
// # Algorithm
// Even though I felt right from the start that brute force can solve this problem, I tried it anyways.
// And within a few seconds the function ''bruteForce'' displayed these 7 solutions:
// 17, 305, 5473, 98209, 1762289, 31622993, 567451585
//
// I wasn't willing to dig deep into mathematical territory and decided to look hard at those numbers ...
// and yes, a pattern revealed:
// 17
// 305 = 17*18 - 1
// 5473 = 305*18 - 17
// 98209 = 5473*18 - 305
// 1762289 = 98209*18 - 5473
// 31622993 = 1762289*18 - 98209
// 567451585 = 31622993*18 - 1762289
//
// That's a surprisingly simple series:
// `x_0 = 1`
// `x_1 = 17`
// `x_n = x_{n-1} * 18 - x_{n-2}`
//
// My code looks a bit messy because of the Hackerrank modifications.
//
// # Hackerrank
// Not only 12 but up to `10^18` triangles have to be found.
// My code precomputes the first 100000 triangles (enough to pass 3 out of 4 test cases) and then
// performs a simple lookup.
//
// All numbers have to be modulo `10^9+7`.
//
// # Alternative
// You can apply Pell's equation to discover this sequence without "taking a hard look".
// And many Fibonacci numbers pop up along the way ...

#include <iostream>
#include <vector>
#include <cmath>

#define ORIGINAL

// find the first sevens solutions in a few seconds
void bruteForce(long long limit)
{
  for (long long base = 2; base < limit; base += 2)
  {
    // both right-angled triangles with side lengths L, h, half
    auto half = base / 2;

    // two potential solutions
    auto height1 = base - 1;
    auto height2 = base + 1;

    // compute L1 and L2
    auto triangle1 = half * half + height1 * height1;
    long long hypo1 = sqrt(triangle1);
    if (hypo1 * hypo1 == triangle1)
      std::cout << hypo1 << " => b=" << base << " h=" << height1 << " diff=" << (base - height1) << std::endl;

    auto triangle2 = half * half + height2 * height2;
    long long hypo2 = sqrt(triangle2);
    if (hypo2 * hypo2 == triangle2)
      std::cout << hypo2 << " => b=" << base << " h=" << height2 << " diff=" << (base - height2) << std::endl;
  }
}

int main()
{
  // precompute the first 10^6 solutions
  std::vector<unsigned long long> solutions;

  // the first seven solutions reveal a pattern ...
  // 17
  // 305 = 17*18 - 1
  // 5473 = 305*18 - 17
  // 98209 = 5473*18 - 305
  // 1762289 = 98209*18 - 5473
  // 31622993 = 1762289*18 - 98209
  // 567451585 = 31622993*18 - 1762289

#ifdef ORIGINAL
  const unsigned int MaxPrecompute = 12;
#else
  const unsigned int MaxPrecompute = 1000000;
#endif

  // first solution is 17
  long long current  = 17;
  solutions.push_back(current);
  // add it to the total
  long long sum      = current;

  // previous solution ("zero-th" solution is 1
  long long previous =  1;
  for (unsigned int i = 2; i <= MaxPrecompute; i++)
  {
    // one more step ...
    auto next = current * 18 - previous;

#ifndef ORIGINAL
    // prevent negative numbers
    const long long Modulo = 1000000007;
    if (next < 0)
      next += Modulo;
#endif

    // next => current => previous
    previous  = current;
    current   = next;

    // add solution
    sum += current;

#ifndef ORIGINAL
    // keep only residue
    sum     %= Modulo;
    current %= Modulo;
#endif

    // store solution
    solutions.push_back(sum);
  }

  unsigned int tests = 1;
  std::cin >> tests;
  while (tests--)
  {
    unsigned long long smallest = 12;
    std::cin >> smallest;

    auto index = smallest - 1;
    // not pre-computed ? => abort
    if (index >= solutions.size())
      return 0;

    // look up result
    std::cout << solutions[index] << std::endl;
  }

  return 0;
}
