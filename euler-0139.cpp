// ////////////////////////////////////////////////////////
// # Title
// Pythagorean tiles
//
// # URL
// https://projecteuler.net/problem=139
// http://euler.stephan-brumme.com/139/
//
// # Problem
// Let (a, b, c) represent the three sides of a right angle triangle with integral length sides.
// It is possible to place four such triangles together to form a square with length c.
//
// ![square](p139.gif)
//
// For example, (3, 4, 5) triangles can be placed together to form a 5 by 5 square with a 1 by 1 hole in the middle
// and it can be seen that the 5 by 5 square can be tiled with twenty-five 1 by 1 squares.
//
// However, if (5, 12, 13) triangles were used then the hole would measure 7 by 7 and these could not be used to tile the 13 by 13 square.
//
// Given that the perimeter of the right triangle is less than one-hundred million, how many Pythagorean triangles would allow such a tiling to take place?
//
// # Solved by
// Stephan Brumme
// June 2017
//
// # Algorithm
// The sides of each triangle are `a <= b < c`.
// In this picture shown above, `c` is the length of the "outer square" and `b - a` is the length of the "hole".
// A tiling exists only if `c` is a multiple of `b - a`, that means ''c % (b - a) == 0''.
//
// For the (3,4,5) triangle: `c mod (b-a) = 5 mod (4-3) = 5 mod 1 = 0` ==> yes, tiling exists
// For the (5,12,13) triangle: `c mod (b-a) = 13 mod (12-5) = 13 mod 7 = 6` ==> no
//
// I generate all basic Pythagorean triplets (see problem 75) and check if ''c % (b - a) == 0''.
// Then all multiples create a valid tiling, too: `k * c mod (k * b - k * a) = k * c mod k * (b - a) = k * (c mod (b - a))` and if `x mod y = 0` then always `kx mod y = 0`.
// There are `\lfloor dfrac{10^8}{a+b+c} \rfloor` valid multiples for each basic triplets.
//
// # Note
// There is a subtle problem with the way basic triplets are generated: `a` and `b` are not always `a <= b` ... sometimes `a > b`.
// So actually the correct test for a valid tiling is ''c % abs(b - a) == 0'' (that's what my ''diff'' variable is for).
//
// When I looked at the valid basic triplets then I saw that `b - a = 1` in all cases. There are actually only 10 basic triplets.

#include <iostream>

// find greatest common divisor
template <typename T>
T gcd(T a, T b)
{
  while (a != 0)
  {
    T c = a;
    a = b % a;
    b = c;
  }
  return b;
}

int main()
{
  unsigned int tests = 1;
  std::cin >> tests;
  while (tests--)
  {
    unsigned int maxLength = 100000000;
    std::cin >> maxLength;

    unsigned int numValid  = 0;
    for (unsigned int m = 2; 2*m*m < maxLength; m++)
      for (unsigned int n = 1; n < m; n++)
      {
        // only valid m and n
        if ((m + n) % 2 != 1)
          continue;
        if (gcd(m, n) != 1)
          continue;

        // compute basic triplet
        auto a = m*m - n*n;
        auto b = 2*m*n;
        auto c = m*m + n*n;
        auto sum = a + b + c;

        // length of a side of the hole
        unsigned int diff = b - a;
        if (a > b)
          diff = a - b;

        // check if outer square is a multiple
        if (diff > 0 && c % diff == 0)
        {
          // number of valid triplets derived from the current basic triplet
          auto moreTriples = maxLength / sum;
          numValid += moreTriples;
        }
      }

    std::cout << numValid << std::endl;
  }

  return 0;
}
