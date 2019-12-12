// ////////////////////////////////////////////////////////
// # Title
// Triangle containment
//
// # URL
// https://projecteuler.net/problem=102
// http://euler.stephan-brumme.com/102/
//
// # Problem
// Three distinct points are plotted at random on a Cartesian plane, for which `-1000 <= x`, `y <= 1000`, such that a triangle is formed.
//
// Consider the following two triangles:
//
// `A(-340,495)`, `B(-153,-910)`, `C(835,-947)`
//
// `X(-175,41)`, `Y(-421,-714)`, `Z(574,-645)`
//
// It can be verified that triangle `\triangle ABC` contains the origin, whereas triangle `\triangle XYZ` does not.
//
// Using [triangles.txt](https://projecteuler.net/project/resources/p102_triangles.txt) (right click and 'Save Link/Target As...'), a 27K text file containing the co-ordinates of one thousand "random" triangles,
// find the number of triangles for which the interior contains the origin.
//
// NOTE: The first two examples in the file represent the triangles in the example given above.
//
// # Solved by
// Stephan Brumme
// May 2017
//
// # Algorithm
// I had a substantial number of lectures on Computer Graphics, so this was a very easy problem ...
// Just for fun I implemented __two__ algorithms: one is based on the dot product and one on barycentric coordinates.
//
// Algorithm ''isInside'' is based on the dot product ( https://en.wikipedia.org/wiki/Dot_product ) and needs the helper function ''dot''.
// A line `(x1,y1)-(x2,y2)` divides the 2D into two disjunct half-planes `P_+` and `P_-`.
// Any point `p_1 \in P_+` has a positive sign whereas any point `p_2 \in P_-` has a negative sign and is zero if `p_3` lies on the line.
// (Actually you can extend this formula to compute the distance of that point from the line).
//
// Computing the dot product of a point `p` regarding each side of the triangle must has the same sign if `p` is inside the triangle.
// My code handle the general case for any `p`, not just the origin at `(0,0)`.
//
// Algorithm ''isInside2'' evaluates the barycentric coordinates ( https://en.wikipedia.org/wiki/Barycentric_coordinate_system ) of a point `p` in relation to a triangle.
// If the barycentric coordinates `a_p`, `b_p` and `c_p` are between 0 and 1 then `p` is inside the triangle.
//
// # Note
// Any point on the edges of a triangle is considered to be inside the triangle.
//
// I prefer the dot product solution because it's less prone to rounding artifacts.
// Actually you can write an integer-only version because it doesn't need division.
//
// # Alternative
// Some people compute the area `A_{ABC}` of the original triangle `\triangle ABC`.
// If `P` is inside `\triangle ABC` then its area equals the sum of the areas of `\triangle ABP`, `\triangle APC` and `\triangle PBC`:
// `A_{ABC} = A_{ABP} + A_{APC} + A_{PBC}`
//
// If you check thousands of points against the same triangle then you should think about finding the bounding box ( https://en.wikipedia.org/wiki/Minimum_bounding_box ).

#include <iostream>

//#define ORIGINAL

// compute dot-product
double dot(double x, double y, double x1, double y1, double x2, double y2)
{
  return (y2 - y1)*(x - x1) + (x1 - x2)*(y - y1);
}

// return true if point (x,y) is inside the triangle (x1,y1),(x2,y2),(x3,y3)
// uses dot-product
bool isInside(double x, double y, double x1, double y1, double x2, double y2, double x3, double y3)
{
  bool sign1 = dot(x,y, x1,y1, x2,y2) >= 0;
  bool sign2 = dot(x,y, x2,y2, x3,y3) >= 0;
  bool sign3 = dot(x,y, x3,y3, x1,y1) >= 0;

  return (sign1 == sign2) && (sign2 == sign3);
}

// same as above but based on barycentric coordinates
bool isInside2(double x, double y, double x1, double y1, double x2, double y2, double x3, double y3)
{
  double denominator = (y2 - y3)*(x1 - x3) + (x3 - x2)*(y1 - y3);

  double a = ((y2 - y3)*(x - x3) + (x3 - x2)*(y - y3)) / denominator;
  double b = ((y3 - y1)*(x - x3) + (x1 - x3)*(y - y3)) / denominator;
  double c = 1 - a - b;

  return 0 <= a && a <= 1 &&
         0 <= b && b <= 1 &&
         0 <= c && c <= 1;
}

int main()
{
  // number of triangles where the origin is inside
  unsigned int numInside = 0;

  unsigned int tests = 1000;
#ifndef ORIGINAL
  std::cin >> tests;
#endif

  while (tests--)
  {
    int x1,y1,x2,y2,x3,y3;

#ifdef ORIGINAL
    // numbers in CSV format
    char comma;
    std::cin >> x1 >> comma >> y1 >> comma >> x2 >> comma >> y2 >> comma >> x3 >> comma >> y3;
#else
    // numbers separated by spaces
    std::cin >> x1 >> y1 >> x2 >> y2 >> x3 >> y3;
#endif

    // both algorithms return the same results
    if (isInside(0,0, x1,y1, x2,y2, x3,y3))
      numInside++;
    //if (isInside2(0,0, x1,y1, x2,y2, x3,y3))
    //  numInside++;
  }

  std::cout << numInside << std::endl;
  return 0;
}
