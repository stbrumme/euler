// ////////////////////////////////////////////////////////
// # Title
// Perfect Square Collection
//
// # URL
// https://projecteuler.net/problem=142
// http://euler.stephan-brumme.com/142/
//
// # Problem
// Find the smallest x + y + z with integers x > y > z > 0 such that x + y, x − y, x + z, x − z, y + z, y − z are all perfect squares.
//
// # Solved by
// Stephan Brumme
// June 2017
//
// # Algorithm
// I have to solve six equations:
// (1) `a^2 = x + y`
// (2) `b^2 = x - y`
// (3) `c^2 = x + z`
// (4) `d^2 = x - z`
// (5) `e^2 = y + z`
// (6) `f^2 = y - z`
//
// And this condition has to be obeyed:
// (7) `x > y > z > 0`
//
// If I add (1) and (2), then I have `x`:
// (8) `a^2 + b^2 = 2x` ==> `x = frac{1}{2}(a^2 + b^2)`
//
// After reordering (1) I get the value of `y`:
// (9) `y = a^2 - x`
//
// The same trick applied to (3):
// (10) `z = c^2 - x`
//
// All equations can be solved if I find valid values for `a^2`, `b^2` and `c^2`.
// However, when computing `d^2`, `e^2` and `f^2` based on (4), (5) and (6) then a few "fake" solutions appear where
// `d^2`, `e^2` and `f^2` are not perfect squares.
//
// My program has three nested loops that iterate over possible values `a`, `b` and `c`.
// A few optimizations are required to finish the algorithm in a reasonable amount of time:
// - `x` must be an integer and from (8) follows that `a^2 + b^2` must be even, that means `a` and `b` are both odd or both even
// - all numbers are positive therefore from (10) follows that `c^2 > x` which is the same as `c > sqrt{x}`
// - instead of checking a number on-the-fly whether it is a perfect square, I precompute a small look-up table
//
// # Alternative
// There are many faster solutions. My short program finishes in less than 0.1 seconds - that's why I don't care about more optimizations.

#include <iostream>
#include <bitset>
#include <cmath>

int main()
{
  // in my tests I found that all tested squares are well below one million
  const unsigned int Limit = 1000000;

  // record all square numbers
  // [n] => [true, if n is a perfect square]
  std::bitset<Limit> isSquare; // default is false / zero
  for (unsigned int i = 1; i*i < isSquare.size(); i++)
    isSquare[i*i] = true;

  // substitute in (3):
  // (9) z = c^2 - x
  for (unsigned int a = 3; ; a++)
  {
    // a and b must be both odd or both even
    unsigned int minB = (a % 2 == 0) ? 2 : 1;
    for (unsigned int b = minB; b < a; b += 2) // keep parity, skip every second number
    {
      // from (8): compute x
      auto x = (a*a + b*b) / 2;
      // from (9): compute y
      auto y = a*a - x;
      // ensure x > y
      if (x <= y)
        break;

      for (unsigned int c = (unsigned int)sqrt(x) + 1; ; c++)
      {
        // from (10): compute z
        auto z = c*c - x;
        // ensure y > z
        if (y <= z)
          break;

        // check whether d^2, e^2 and f^2 are perfect squares
        if (isSquare[x - z] &&
            isSquare[y + z] &&
            isSquare[y - z])
        {
          // found a solution, stop program
          std::cout << x + y + z << std::endl;
          return 0;
        }
      }
    }
  }

  // never reached
}
