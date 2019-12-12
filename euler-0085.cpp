// ////////////////////////////////////////////////////////
// # Title
// Counting rectangles
//
// # URL
// https://projecteuler.net/problem=85
// http://euler.stephan-brumme.com/85/
//
// # Problem
// By counting carefully it can be seen that a rectangular grid measuring 3 by 2 contains eighteen rectangles:
//
// ![Examples](p085.gif)
//
// Although there exists no rectangular grid that contains exactly two million rectangles, find the area of the grid with the nearest solution.
//
// # Solved by
// Stephan Brumme
// March 2017
//
// # Algorithm
// When you look at the "one-dimensional" case (an area with only 1 row):
// ||   3  ||    6       ||  4     ||
// || area || rectangles ||  total ||
// || 1x1  || 1          ||    1   ||
// || 2x1  || 2+1        ||    3   ||
// || 3x1  || 3+2+1      ||    6   ||
// || 4x1  || 4+3+2+1    ||   10   ||
//
// These are the triangle numbers `T(x) = x * dfrac{x+1}{2}`, see https://en.wikipedia.org/wiki/Triangular_number
// The same pattern appears in the 2D case with more than 1 row.
// An area `A` contains:
// `A(x,y) = T(x) * T(y) = dfrac{x(x+1)}{2} * dfrac{y(y+1)}{2}`
//
// `= dfrac{1}{4} xy * (x+1)(y+1)`
//
// Under the assumption that `y > x` I iterate over all `x` and `y` until I find the first area exceeding the limit.
// Each subsequent grid can't yield a better solution because of `A(x,y+1) > A(x,y)`.
//
// The grid with the nearest solution is the grid where the number of rectangles `r` is is closest to 2000000, i.e. where `abs(r - 2000000)` is minimized.

#include <iostream>
#include <cmath>

int main()
{
  unsigned int tests = 1;
  std::cin >> tests;
  while (tests--)
  {
    unsigned int target = 2000000;
    std::cin >> target;

    // assume x <= y, therefore x <= sqrt(limit)
    unsigned int root = sqrt(target);
    unsigned int bestRectangles = 0;
    unsigned int bestArea       = 0;
    for (unsigned int x = 1; x <= root + 1; x++) // allow slight overshooting
    {
      // start with a sqaure
      unsigned int y = x;
      // number of rectangles
      unsigned int rectangles = 0;

      // slowly increase y until too many rectangle in the grid
      do
      {
        unsigned int area = x * y;

        // the formula derived above
        rectangles = x * (x + 1) * y * (y + 1) / 4;

        // closer to desired number of rectangles than before ?
        if (abs(bestRectangles - target) > abs(rectangles - target))
        {
          bestRectangles = rectangles;
          bestArea       = area;
        }

        // prefer larger areas, too (additional requirement of Hackerrank)
        if (abs(bestRectangles - target) == abs(rectangles - target) && bestArea < area)
          bestArea = area;

        y++;
      } while (rectangles < target);

      // just a speed-up ... abortion when the inner loop exited with a square area x*y
      // => it means that no further solutions possible, area already too large
      if (y == x + 1) // plus one because y was incremented before leaving the inner loop
        break;
    }
    std::cout << bestArea << std::endl;
  }
  return 0;
}
