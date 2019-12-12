// ////////////////////////////////////////////////////////
// # Title
// Perfect right-angled triangles
//
// # URL
// https://projecteuler.net/problem=218
// http://euler.stephan-brumme.com/218/
//
// # Problem
// Consider the right angled triangle with sides `a=7`, `b=24` and `c=25`. The area of this triangle is 84, which is divisible by the perfect numbers 6 and 28.
// Moreover it is a primitive right angled triangle as `gcd(a,b)=1` and `gcd(b,c)=1`.
// Also `c` is a perfect square.
//
// We will call a right angled triangle perfect if
// - it is a primitive right angled triangle
// - its hypotenuse is a perfect square
//
// We will call a right angled triangle super-perfect if
// - it is a perfect right angled triangle and
// - its area is a multiple of the perfect numbers 6 and 28.
//
// How many perfect right-angled triangles with `c <= 10^16` exist that are not super-perfect?
//
// # Solved by
// Stephan Brumme
// August 2017
//
// # Algorithm
// I wrote a small program to check all such triangle with `c <= 10^8` and found zero perfect right-angled triangles that are not super-perfect.
// See below for the simple code, most of it was copied from problem 86.
//
// Even trying some randomized values for ''m'' and ''n'' failed to find such a triangle.
//
// Then I went back to my highly valued "paper and pencil" technique and found a relationship:
// each primitive triple is defined by
// (1) `a = m^2 - n^2`
// (2) `b = 2mn`
// (3) `c = m^2 + n^2`
// (4) `gcd(m,n) = 1`
// (5) `(m mod 2) != (n mod 2)`
// Those equations were already part of multiple Project Euler problems, such as problem 86.
//
// This time there are more restrictions:
// - `c` is a perfect square, so there is an integer `d` such that `c = d^2`
// - the area of each right-angled triangle is `A = ab / 2`
// - `A mod 6 == 0` and `A mod 28 == 0`
//
// The last restriction can be simplified: the least common multiple `lcm(6,28) = 6 * 28 / gcd(6,28) = 84`, therefore `A mod 84 == 0` and
// the area `A` must be a multiple of 84:
// (6) `A == 0 mod 84`
// (7) `ab/2 == 0 mod 84`
// (8) `ab == 0 mod 168`
//
// I have to find all perfect right-angled triangle where the area is a multiple of 168 (and the hypotenuse `< 10^16`)
//
// Since `c` is a perfect square with `c = d^2` I can rewrite `c = m^2 + n^2` as
// (9) `d^2 = m^2 + n^2`
//
// This is actually a primitive triple again - because of `gcd(m, n) = 1`. So can repeat the procedure again and there must be some `x` and `y` such that
// (10) `m = x^2 - y^2`
// (11) `n = 2xy`
// (12) `d = x^2 + y^2`
//
// Substituting `m` and `n` in (1) and (2):
// (13) `a = (x^2 - y^2)^2 - (2xy)^2`
// (14) `b = 2 * (x^2 - y^2) * 2xy`
//
// Therefore the area of these triangles becomes (see (8) ):
// (15) `((x^2 - y^2)^2 - (2xy)^2) * 2 * (x^2 - y^2) * 2xy == 0 mod 168`
// (16) `((x^2 - y^2)^2 - (2xy)^2) * (x^2 - y^2) * xy == 0 mod 42`
//
// I wrote two nested loops iterating over all `42^2` basic pairs `(x mod 42, y mod 42)` (see ''countNotMod42'') - and actually of them produce zero in equation (16).
// That means that there are no solutions, no matter whether the limit is `10^8`, `10^16` or `infinity`.
//
// # Alternative
// Nayuki's proof is almost identical to my concept. Some of my ideas are solved by a program (multiple of 42) whereas he showed the same in a mathematical way.
// It's also important to note that he found his proof a few years earlier.
// I was kind of surprised when my program returned zero and even suspected a bug because I didn't fully trust ''countNotMod42''.
//
// # Note
// ''countNotMod42'' fails when ''x'' or ''y'' are ''int'' (or ''unsigned'') instead of ''long long'' because of overflows. It cost me half an hour to realize that problem.
// This function also contains a constant named ''Multiplier'' which can be ignored ==> set it to 1.

#include <iostream>
#include <cmath>

// return number of pairs (x,y) such that equation (16) is not zero mod 42
unsigned int countNotMod42()
{
  unsigned int result = 0;

  const auto Multiplier = 1; // no more than 5 because then long long overflows
  for (long long x = 0; x < 42 * Multiplier; x++)
    for (long long y = 0; y < 42 * Multiplier; y++)
    {
      auto zero = ((x*x - y*y)*(x*x - y*y) - 2*x*y*2*x*y) * (x*x - y*y) * x * y;
      if (zero % 42 != 0)
        result++;
    }
  return result;
}

// greatest common divisor
template <typename T>
T gcd(T x, T y)
{
  while (x != 0)
  {
    auto temp = x;
    x = y % x;
    y = temp;
  }
  return y;
}

int main()
{
  // count how many possible pairs (x,y) could generate (a,b,c) according to equation (16)
  std::cout << countNotMod42() << std::endl;
  return 0;

//#define SEARCH
#ifdef  SEARCH
  // ---------- try to find a non super-perfect triangle up to 10^8 ---------
  // note: code currently not reached because the program exited three lines ago
  unsigned long long limit = 100000000;
  std::cin >> limit;

  unsigned long long countNotSuperPerfect = 0;

  // find basic Pythagorean triples (code copied from problem 86)
  for (unsigned long long m = 1; m <= sqrt(2*limit); m++)
    for (unsigned long long n = 1; n < m; n++)
    {
      if (m % 2 == n % 2)
        continue;
      if (gcd(m, n) != 1)
        continue;

      // two sides
      auto a = m*m - n*n;
      auto b = 2*m*n;
      auto c = m*m + n*n;
      if (c > limit)
        break;

      // is c a perfect square ?
      unsigned long long cRoot = sqrt(c);
      if (cRoot * cRoot != c)
        continue;

      // is area a multiple of 6 and 28 ?
      auto area = a * b / 2;
      if (area % 6 != 0 || area % 28 != 0) // can be combined: area % lcm(6, 28) = area % 84
      {
        countNotSuperPerfect++;
        continue;
      }
    }

  // show result
  std::cout << countNotSuperPerfect << std::endl;
  return 0;
#endif
}
