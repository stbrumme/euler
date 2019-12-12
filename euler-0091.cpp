// ////////////////////////////////////////////////////////
// # Title
// Right triangles with integer coordinates
//
// # URL
// https://projecteuler.net/problem=91
// http://euler.stephan-brumme.com/91/
//
// # Problem
// The points `P(x1,y1)` and `Q(x2,y2)` are plotted at integer co-ordinates and are joined to the origin, `O(0,0)`, to form a triangle `\triangle OPQ`.
//
// ![coordinate system](p091_1.gif)
//
// There are exactly fourteen triangles containing a right angle that can be formed when each co-ordinate lies between 0 and 2 inclusive;
// that is, `0 <= x1, y1, x2, y2 <= 2`.
//
// ![coordinate system](p091_2.gif)
//
// Given that `0 <= x1, y1, x2, y2 <= 50`, how many right triangles can be formed?
//
// # Solved by
// Stephan Brumme
// March 2017
//
// # Algorithm
// The location of `O` was fixed at `(0,0)` but `P` and `Q` can be anywhere (but `O!=P!=Q`).
// To simplify my algorithm I define that the right angle is always at `O` or `P` and never at `Q`.
// The size of the grid is 50x50, each side has length `size=50`.
//
// I identified 4 cases:
// 1. The right angle is at the origin `O`. `P` and `Q` can be anywhere on the x-/y-axis:
//    `P(0<p_x<=size, 0)` and `Q(0, 0<q_y<=size)` ==> that's `size^2` different triangles.
//
// 2. The right angle is on the x-axis at `P(0<p_x<=size, 0)`. Any `Q` with the same x-value makes a valid triangle:
//    `P(0<p_x<=size, 0)` and `Q(p_x, 0<q_y<=size)` ==> that's `size^2` different triangles.
//
// 3. The right angle is on the y-axis at `P(0, 0<p_y<=size)`. Any `Q` with the same y-value makes a valid triangle:
//    `P(0, 0<p_y<=size)` and `Q(0<q_x<=size, p_y)` ==> that's `size^2` different triangles.
//
// 4. The right angle is "inside" the grid at `P(0<p_x<=size, 0<p_y<=size)`.
//
// The fourth case is a more challenging case and will be discussed in detail:
// The right angle is enclosed by two vectors `\vec{a} = \vec{PO}` and `\vec{b} = \vec{PQ}`.
// If they are perpendicular `\vec{a} \bot \vec{b}`, then their dot product is zero: `\vec{a} \cdotp \vec{b} = a_{x}b_x + a_{y}b_y = 0`
//
// The task is to find for a given point `P(p_x, p_y)` each `Q(q_x, q_y)` such that `p_x(q_x - p_x) + p_y(q_y - p_y) = 0`.
// For example `P(2,2)` and `Q(3,1)` create a valid right angle:
// `p_x(q_x - p_x) + p_y(q_y - p_y) = 2(3-2) + 2(1-2) = 2 - 2 = 0`
//
// The dot product can be transformed to
// `p_x(q_x - p_x) = -p_y(q_y - p_y)`
// This formula is obivously true for `q_x = p_x` and `q_y = p_y`.
// Even more, it's also true for `q_x = p_x + k \cdot p_y` and `q_y = p_y - k \cdot p_x` (last line was just the special case `k=0`).
// `p_x(p_x + k p_y - p_x) = -p_y(p_y - k p_x - p_y)`
// `p_x(k p_y) = -p_y(-k p_x)`
// `kp_{x}p_y = kp_{x}p_y`
//
// Two nested ''for''-loops produce all integer combinations of `0 < p_x <= size` and `0 < p_y <= size`.
// We only want integer solutions for `q_x` and `q_y`, too.
// But if `k` is an integer, too, then we would miss some valid `Q` - e.g. for `P(2,2)` and `Q(3,1)` it follows that `k=frac{1}{2}`.
// We get all integer solutions if `k` is a multiple of `dfrac{1}{gcd(p_x,p_y)}` (the Greatest Common Divisor, see https://en.wikipedia.org/wiki/Greatest_common_divisor )
// For `P(2,2)` we get `dfrac{1}{gcd(2,2)}=dfrac{1}{2}`
//
// Enough mathematics, let's dive into the code ... cases 1 to 3 don't need much of an explanation.
// Then follows two nested ''for''-loop that analyze each `P(p_x,p_y)` of case 4.
// ''factor'' is the ''gcd(p_x,p_y)'' such that `k_{n+1} = k_n + 1/factor`
// The point `Q` is found by repeated adding ''p_y/factor'' to ''p_x'' and subtracting ''p_x/factor'' from ''p_y'' (for k > 0)
// until any coordinate leaves the grid. The same procedure is done with inverse sign for k < 0.
//
// A simple speed-up was achieved by observing that the number of valid `Q` is the same for `P(p_x, p_y)` and `P(p_y, p_x)`.
// That means that the two nested loops only need to considered the bottom-right triangle of the grid: `P(p_x, p_y<=p_x)`.
// For `P(p_x, p_y<p_x)` I multiply the number of `Q`s by 2 - but not when `p_x=p_y`.
//
// # Alternative
// Most other solutions replace my inner-most ''while''-loops by a closed formula:
// ''min(y * factor / x, (size - x) * factor / y) * 2''
// Noone really explains that formula and surprisingly it isn't faster on my machine for ''size <= 2500''.
//
// The original problem can be solved by brute-force, too (four nested loops for ''p_x'', ''p_y'', ''q_x'', ''q_y'').
//
// # Hackerrank
// Grids can have an edge length of up to 2500.
// The brute-force approach fails of course (time-out) while my code finishes in less than 0.22 seconds.

#include <iostream>
#include <algorithm>

// greatest common divisor
unsigned int gcd(unsigned int a, unsigned int b)
{
  while (a != 0)
  {
    unsigned int c = a;
    a = b % a;
    b = c;
  }
  return b;
}

int main()
{
  unsigned int size = 50;
  std::cin >> size;

  // triangles where right angle is in the origin
  unsigned int result = size*size;

  // plus triangles where the right angle is located on the x-axis
  result += size*size;
  // plus triangles where the right angle is located on the y-axis
  result += size*size;

  // now all triangles where the right angle at point P(x > 0, y <= x)
  // that's the triangle in the bottom-right half, denoted by slashes
  // ^   /
  // |  //
  // | ///
  // |////
  // +--->

  for (unsigned int p_x = 1; p_x <= size; p_x++)
    for (unsigned int p_y = 1; p_y <= p_x; p_y++)
    {
      // reduce to a proper fraction
      unsigned int factor = gcd(p_x, p_y);
      unsigned int deltaX = p_x / factor;
      unsigned int deltaY = p_y / factor;

      unsigned int found = 0;

      // assume Q is "below" P
      int q_x = p_x - deltaY;
      int q_y = p_y + deltaX;
      while (q_x >= 0 && q_y <= (int)size)
      {
        found++;
        q_x -= deltaY;
        q_y += deltaX;
      }

      // assume Q is "above" P
      q_x = p_x + deltaY;
      q_y = p_y - deltaX;
      while (q_y >= 0 && q_x <= (int)size)
      {
        found++;
        q_x += deltaY;
        q_y -= deltaX;
      }

      // mirror along y=x when p_y < p_x
      if (p_x != p_y)
        found *= 2;

      result += found;
    }

  std::cout << result << std::endl;

  return 0;
}
