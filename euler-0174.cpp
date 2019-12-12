// ////////////////////////////////////////////////////////
// # Title
// Counting the number of "hollow" square laminae that can form one, two, three, ... distinct arrangements
//
// # URL
// https://projecteuler.net/problem=174
// http://euler.stephan-brumme.com/174/
//
// # Problem
// We shall define a square lamina to be a square outline with a square "hole" so that the shape possesses vertical and horizontal symmetry.
//
// Given eight tiles it is possible to form a lamina in only one way: 3x3 square with a 1x1 hole in the middle.
// However, using thirty-two tiles it is possible to form two distinct laminae.
//
// ![tiles](https://projecteuler.net/project/images/p173_square_laminas.gif)
//
// If `t` represents the number of tiles used, we shall say that `t = 8` is type `L(1)` and `t = 32` is type `L(2)`.
//
// Let `N(n)` be the number of `t <= 1000000` such that `t` is type `L(n)`; for example, `N(15) = 832`.
//
// What is `sum{N(n)}` for `1 <= n <= 10`?
//
// # Solved by
// Stephan Brumme
// May 2017
//
// # Algorithm
// I copied most of my solution from the previous problem (problem 173).
// A second pass iterates over all solutions and counts which number of tiles has between 1 and 10 solutions. This information is stored in ''result''.
// Then we just have to look up the relevant element, e.g. ''result[1000000]'' for the original problem.
//
// # Hackerrank
// I could have written my solution without the ''result'' array because only ''result[1000000]'' was relevant.
// However, Hackerrank queries thousands of other elements from ''result''.
// Due to heavy I/O processing, which is kind of slow in C++, I barely solve the second test case.

#include <iostream>
#include <vector>
#include <algorithm>

int main()
{
  const unsigned int limit = 1000000;

  // count different layouts for a number of tiles
  std::vector<unsigned int> solutions(limit + 1, 0);

  // start with smallest outer ring
  for (unsigned int outer = 3; ; outer++)
  {
    unsigned int sum = 0;
    // add as many inner rings as possible
    for (unsigned int inner = outer; inner >= 3; inner -= 2)
    {
      // tiles needed to create one ring whose sides are "inner" tiles long
      unsigned int ring = 4 * (inner - 1);

      // runnng out of tiles ?
      if (sum + ring > limit)
        break;

      // add valid ring
      sum += ring;
      solutions[sum]++;
    }

    // no more inner rings possible, abort
    if (sum == 0)
      break;
  }

  // pre-process all possible answers
  std::vector<unsigned int> result(solutions.size());
  unsigned int one2ten = 0;
  for (unsigned int i = 0; i < solutions.size(); i++)
  {
    auto s = solutions[i];
    if (s >= 1 && s <= 10)
      one2ten++;

    result[i] = one2ten;
  }

  // process test cases
  unsigned int tests;
  std::cin >> tests;
  while (tests--)
  {
    unsigned int last = limit;
    std::cin >> last;
    // simple lookup
    std::cout << result[last] << std::endl;
  }

  return 0;
}
