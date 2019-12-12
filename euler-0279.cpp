// ////////////////////////////////////////////////////////
// # Title
// Triangles with integral sides and an integral angle
//
// # URL
// https://projecteuler.net/problem=279
// http://euler.stephan-brumme.com/279/
//
// # Problem
// How many triangles are there with integral sides, at least one integral angle (measured in degrees), and a perimeter that does not exceed `10^8` ?
//
// # Solved by
// Stephan Brumme
// December 2017
//
// # Algorithm
// Niven's theorem (see https://en.wikipedia.org/wiki/Niven's_theorem ) states that for rational angles `sin(alpha)` the only rational values are `\{ 0, +1/2, -1/2, +1, -1 \}`.
// Since `sin(90° + alpha) = cos(alpha)` this theorem can be applied to `cos(alpha)`, too.
//
// Thus only 3 angles can be part of a valid triangle:
// (1) `cos( 60°) = +1/2`
// (2) `cos( 90°) =   0`
// (3) `cos(120°) = -1/2`
//
// Each triangle satisfies (see https://en.wikipedia.org/wiki/Law_of_cosines ):
// (4) `c^2 = a^2 + b^2 - 2ab cos(gamma)`
//
// If `gamma =  60°` then (4) can be simplified to
// (5) `c^2 = a^2 + b^2 - ab`
//
// If `gamma =  90°` then (4) can be simplified to
// (6) `c^2 = a^2 + b^2`
//
// If `gamma = 120°` then (4) can be simplified to
// (7) `c^2 = a^2 + b^2 + ab`.
//
// These equations have their own Wikipedia pages, too: https://en.wikipedia.org/wiki/Eisenstein_triple and https://en.wikipedia.org/wiki/Pythagorean_triple .
// But more interesting is https://en.wikipedia.org/wiki/Integer_triangle which gives away nice formulas to generate all such triangles.
// The formulas need two variables `m` and `n` where `0 < n < m`.
//
// The case `cos(60°)` boils down to:
// `a = m^2 - mn + n^2`
// `b = 2mn - n^2`
// `c = m^2 - n^2`
//
// I solved `cos(90°)` in problem 75 (see https://en.wikipedia.org/wiki/Pythagorean_triple ):
// `a = m^2 - n^2`
// `b = 2mn`
// `c = m^2 + n^2`
//
// And finally `cos(120°)`:
// `a = m^2 + mn + n^2`
// `b = 2mn + n^2`
// `c = m^2 - n^2`
//
// The triples `(a,b,c)` are not necessarily basic triples - but their greatest common divisor is always either 1 or 3.
// Now I have all the tools to generate all basic triples. Those basic triples and all their multiples are solutions.
// There are `10^8 / perimeter` multiples of each basic triples which satisfy the condition that the perimeter must not exceed `10^8`.
//
// The functions ''search60()'', ''search90()'' and  ''search120()'' implement each set of equations for `60°`, `90°` and `120°`.
//
// # Note
// Fortunately I didn't find a triangle with angles `60° + 90° + 30°` and integer sides.
// If there were any, I would have needed to avoid double-counting (as they would be counted as `60°` and `90°` separately).
//
// Since today is my lucky day, I added a few OpenMP pragmas which reduce the execution time from 4 to less than 1 second on my computer (enable ''#define PARALLEL'').

#include <iostream>
#include <cmath>

#define PARALLEL
const auto numCores = 8; // 4 = four cores, 8 = eight cores, etc.

// ---------- code copied from my toolbox ----------

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

// ---------- problem-specific code ----------

// 90 degrees (based on code from problem 75)
unsigned int search90(unsigned int limit)
{
  unsigned int result = 0;

  unsigned int last = sqrt(limit / 2);
#ifdef PARALLEL
#pragma omp parallel for num_threads(numCores) reduction(+:result) schedule(dynamic)
#endif
  for (unsigned int m = 2; m < last; m++)
    for (unsigned int n = (m & 1) + 1; n < m; n += 2) // m + n must be odd
    {
      // only valid m and n
      if (gcd(m, n) != 1)
        continue;

      // compute basic triplet
      auto a = m*m - n*n;
      auto b = 2*m*n;
      auto c = m*m + n*n;

      // too large ?
      auto sum = a + b + c;
      if (sum > limit)
        break;

      // all its multiples are valid, too
      auto numMultiples = limit / sum;
      result += numMultiples;
    }

  return result;
}

// 60 degrees
unsigned int search60(unsigned int limit)
{
  unsigned int last = sqrt(limit*3/2);
  unsigned int result = 0;
#ifdef PARALLEL
#pragma omp parallel for num_threads(numCores) reduction(+:result) schedule(dynamic)
#endif
  for (unsigned int m = 2; m < last; m++)
    for (unsigned int n = 1; 2*n <= m; n++)
    {
      // only valid m and n
      if (gcd(m, n) != 1)
        continue;

      // compute next triplet
      auto a = m*m - m*n + n*n;
      auto b = 2*m*n - n*n;
      auto c = m*m - n*n;

      auto sum = a + b + c;
      // divide by their greatest common divisor to get the basic triplet
      // gcd(a,b,c) is either 1 or 3
      if (a % 3 == 0 && b % 3 == 0 && c % 3 == 0)
        sum /= 3;
      if (sum > 3*limit)
        break;

      if (sum <= limit)
      {
        // all its multiples are valid, too
        auto numMultiples = limit / sum;
        result += numMultiples;
      }
    }

  return result;
}

// 120 degrees
unsigned int search120(unsigned int limit)
{
  unsigned int result = 0;

  unsigned int last = sqrt(limit*3/2);
#ifdef PARALLEL
#pragma omp parallel for num_threads(numCores) reduction(+:result) schedule(dynamic)
#endif
  for (unsigned int m = 2; m < last; m++)
    for (unsigned int n = 1; 2*n <= m; n++)
    {
      // only valid m and n
      if (gcd(m, n) != 1)
        continue;

      // compute next triplet
      auto a = m*m + m*n + n*n;
      auto b = 2*m*n + n*n;
      auto c = m*m - n*n;

      // skip mirrored triangles
      if (b > c)
        break;
      // note: a is always the longest side

      auto sum = a + b + c;
      // divide by their greatest common divisor to get the basic triplet
      // gcd(a,b,c) is either 1 or 3
      if (a % 3 == 0 && b % 3 == 0 && c % 3 == 0)
        sum /= 3;
      if (sum > 3*limit)
        break;

      // all its multiples are valid, too
      auto numMultiples = limit / sum;
      result += numMultiples;
    }

  return result;
}

int main()
{
  unsigned int limit = 100000000;
  std::cin >> limit;

  auto num60  = search60 (limit);
  auto num90  = search90 (limit);
  auto num120 = search120(limit);

  auto result = num60 + num90 + num120;
  std::cout << result << std::endl;
  return 0;
}
