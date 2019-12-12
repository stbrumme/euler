// ////////////////////////////////////////////////////////
// # Title
// Integer Ladders
//
// # URL
// https://projecteuler.net/problem=309
// http://euler.stephan-brumme.com/309/
//
// # Problem
// In the classic "Crossing Ladders" problem, we are given the lengths `x` and `y` of two ladders resting on the opposite walls of a narrow, level street.
// We are also given the height `h` above the street where the two ladders cross and we are asked to find the width of the street (`w`).
//
// ![crossing ladders](p309_ladders.gif)
//
// Here, we are only concerned with instances where all four variables are positive integers.
// For example, if `x = 70`, `y = 119` and `h = 30`, we can calculate that `w = 56`.
//
// In fact, for integer values `x`, `y`, `h` and `0 < x < y < 200`, there are only five triplets `(x,y,h)` producing integer solutions for `w`:
// `(70, 119, 30)`, `(74, 182, 21)`, `(87, 105, 35)`, `(100, 116, 35)` and `(119, 175, 40)`.
//
// For integer values `x`, `y`, `h` and `0 < x < y < 1 000 000`, how many triplets `(x,y,h)` produce integer solutions for `w`?
//
// # Solved by
// Stephan Brumme
// July 2017
//
// # Algorithm
// Each ladders forms two right triangles:
// - the first triangle consists of the ladder `x` (or `y`), the height `X` (or `Y`) where the ladder touches the wall and the width `w` of the lane.
// - the second triangle consists of the lower part of the ladder  `x_1` (or `y_1`), the height `h` where both ladders cross and a part `w_1` (or `w_2`) of the lane.
//
// <![CDATA[
//  <svg width="10em" height="10em">
//   <line x1= "1%" y1= "1%" x2= "1%" y2="99%" stroke="black" stroke-width="1" />
//   <line x1= "1%" y1="99%" x2="99%" y2="99%" stroke="black" stroke-width="1" />
//   <line x1="99%" y1= "1%" x2="99%" y2="99%" stroke="black" stroke-width="1" />
//   <line x1= "1%" y1="99%" x2="99%" y2="39%" stroke="black" stroke-width="1" />
//   <line x1= "1%" y1="15%" x2="99%" y2="99%" stroke="black" stroke-width="1" />
//   <line x1="58%" y1="99%" x2="58%" y2="64%" stroke="black" stroke-width="1" />
//   <text x="85%" y="97%" style="font-size: 1em">&beta;</text>
//   <text x= "5%" y="15%" style="font-size: 1em">Y</text>
//   <text x="15%" y="97%" style="font-size: 1em">&alpha;</text>
//   <text x="90%" y="39%" style="font-size: 1em">X</text>
//   <text x="35%" y="95%" style="font-size: 1em">w1</text>
//   <text x="65%" y="95%" style="font-size: 1em">w2</text>
//   <text x="60%" y="80%" style="font-size: 1em">h</text>
//  </svg>
// ]]>
//
// It can be seen that
// (1) `w = w_1 + w_2`.
//
// The angle `alpha` between ladder `x` and the lane is part of both triangle formed by the first ladder:
// (2) `cos alpha = dfrac{X}{w}` and `cos alpha = dfrac{h}{w_1}`
//
// The same can be said about an angle `beta` of ladder `y`:
// (3) `cos beta = dfrac{Y}{w}` and `cos beta = dfrac{h}{w_2}`
//
// Both equations of (2) can be merged (and the same for (3) ):
// (4) `dfrac{X}{w} = dfrac{h}{w_1}`
//
// (5) `dfrac{Y}{w} = dfrac{h}{w_2}`
//
// Let's transform (4) and (5), merge them and solve for `w_1`:
// (6) `w * h = X * w_1`
// (7) `w * h = Y * w_2`
// (8) `X * w_1 = Y * w_2`
// (9) `w_1 = frac{Y * w_2}{X}`
//
// Combining (1) and (9):
// (10) `w = frac{Y * w_2}{X} + w_2`
//
// Transform (7) and substitute in (10):
// (11) `w = frac{Y * w_2}{h}`
// (12) `frac{Y * w_2}{h} = frac{Y * w_2}{X} + w_2`
//
// Simplify:
// (13) `frac{Y * w_2}{h} = frac{Y * w_2 + X * w_2}{X}`
// (14) `frac{Y}{h} = frac{Y + X}{X}`
//
// And solve for `h`:
// (15) `h = dfrac{Y}{frac{Y + X}{X}}`
//
// (16) `h = dfrac{XY}{Y + X}`
//
// That means that I have to find the height above ground where both ladders touch the wall.
// If `frac{XY}{Y + X}` is an integer, then I have a solution.
//
// Wikipedia has the formula in a different notation (see https://en.wikipedia.org/wiki/Crossed_ladders_problem , they replaced `X` by `A` and `Y` by `B`):
// `dfrac{1}{X} + dfrac{1}{Y} = dfrac{1}{h}`
//
// My program generates all Pythagorean triples (see https://en.wikipedia.org/wiki/Pythagorean_triple , my code is based on my solution for problem 75).
// A container ''sides'' enumerates for each of the legs the length of the other leg (if `c^2 = a^2 + b^2` then it stores ''a'' in ''sides[b]'' and ''b'' in ''sides[a]'').
// This is done for all basic triples as well as for all multiples.
//
// In the second phase all values of ''sides[i]'' are compared to each other:
// - they are triangles where one side is equal, that's the shared width `w` of the lane
// - the other leg is the height of the point where each ladder touches a wall
// - the height `h` of their intersection is `h = dfrac{XY}{Y + X}` according to equation (16)
// - and the height `h` is only an integer if `XY` is a multiple of `Y + X`
//
// # Note
// There is probably a shorter way to derive equation (16). However, any website about the "Crossing Ladders Problem" doesn't show
// how they arrive at that formula, they present it as "obviously easy". It wasn't easy for me and took quite a few attempts to get it right.
//
// On the other hand, it's an easy task for Wolfram Alpha ... it can do all the heavy lifting (unfortunately without printing intermediate steps):
// http://www.wolframalpha.com/input/?i=h%2Fu%3Da%2Fw+and+h%2Fv%3Db%2Fw+and+u%2Bv%3Dw

#include <iostream>
#include <vector>
#include <algorithm>

// most of the code is based on problem 75

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
  // 10^6
  unsigned int limit = 1000000;
  std::cin >> limit;

  std::vector<std::vector<unsigned int>> sides(limit);

  // generate all Pythagorean triples (see https://en.wikipedia.org/wiki/Pythagorean_triple )
  for (unsigned int m = 2; m*m < limit; m++)
    for (unsigned int n = (m & 1) + 1; n < m; n += 2)
    {
      // only valid m and n
      if (gcd(m, n) != 1)
        continue;

      // compute basic triple
      auto a = m*m - n*n;
      auto b = 2*m*n;
      auto c = m*m + n*n;

      // and all multiples
      for (unsigned int k = 1; k*c < limit; k++) // longest side must be < 10^6
      {
        // either height is shorter than width ...
        sides[k*a].push_back(k*b);
        // or the other way around
        sides[k*b].push_back(k*a);
      }
    }

  auto count = 0;
  // process all triangles with one identical side (the width of the street)
  for (const auto& current : sides)
  {
    // compare all potential heights
    for (size_t left = 0; left < current.size(); left++)
      for (size_t right = left + 1; right < current.size(); right++)
      {
        auto X = current[left];
        auto Y = current[right];

        // see "Algorithm" for an explanation of the formula
        if ((X * (unsigned long long)Y) % (X + Y) == 0)
          count++;
        // note: need 64 bit multiplication because of potential overflows
      }
  }

  // display result
  std::cout << count << std::endl;
  return 0;
}
