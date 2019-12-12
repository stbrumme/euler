// ////////////////////////////////////////////////////////
// # Title
// Investigating multiple reflections of a laser beam
//
// # URL
// https://projecteuler.net/problem=144
// http://euler.stephan-brumme.com/144/
//
// # Problem
// In laser physics, a "white cell" is a mirror system that acts as a delay line for the laser beam.
// The beam enters the cell, bounces around on the mirrors, and eventually works its way back out.
//
// The specific white cell we will be considering is an ellipse with the equation `4x^2 + y^2 = 100`
//
// The section corresponding to `-0.01 <= x <= +0.01` at the top is missing, allowing the light to enter and exit through the hole.
//
// ![white cell](p144_1.gif) ![bounces](p144_2.gif)
//
// The light beam in this problem starts at the point `(0.0,10.1)` just outside the white cell, and the beam first impacts the mirror at `(1.4,-9.6)`.
//
// Each time the laser beam hits the surface of the ellipse, it follows the usual law of reflection "angle of incidence equals angle of reflection."
// That is, both the incident and reflected beams make the same angle with the normal line at the point of incidence.
//
// In the figure on the left, the red line shows the first two points of contact between the laser beam and the wall of the white cell;
// the blue line shows the line tangent to the ellipse at the point of incidence of the first bounce.
//
// The slope `m` of the tangent line at any point `(x,y)` of the given ellipse is: `m = -4x/y`
//
// The normal line is perpendicular to this tangent line at the point of incidence.
//
// The animation on the right shows the first 10 reflections of the beam.
//
// How many times does the beam hit the internal surface of the white cell before exiting?
//
// # Solved by
// Stephan Brumme
// June 2017
//
// # Algorithm
// I solved this problem using a combination of vector computations and playing with the formula of an ellipse.
//
// The structs ''Vector'' and ''Point'' are very simple. I could get rid of them and just use simple variables
// but I like to have my code clean and organized instead of short and unreadable.
//
// Each iteration of the main loop performs these tasks:
// 1. check the intersection ''to'' whether it belongs to the hole on top of the ellipse (and isn't actually a hole).
// 2. find the normal at point ''to''
// 3. reflect the ray's ''direction'' in relation to the normal
// 4. compute next intersection of the new ray
//
// I consider every intersection between `-0.01 <= x <= +0.01` and `y > 9.9` to be a valid exit point.
// (it's even sufficient to ask for `y > 0`, but don't change the `9.9` to `10` because of the curving of the ellipse).
//
// The reflection can be found very easily with the dot product (nice and short explanation: http://paulbourke.net/geometry/reflected/ )
//
// It took me quite some time to come up with a formula that computes the next intersection.
//
// The ray has a slope `s = y / x` and any point on the ray is described as
// (1) `y_2 = s(x_2 - x_1) + y_1`
//
// The ellipse was defined as `4x^2 + y^2 = 100` which is
// (2) `dfrac{x^2}{5} + dfrac{y^2}{10} = 1` (that means `a = 5` and `b = 10`)
//
// The first (already known) intersection occurs at
// (3) `4x_1^2 + y_1^2 = 1`
//
// The second (unknown) intersection will be at
// (4) `4x_2^2 + y_2^2 = 1`
//
// I replace in the latter formula (4) the variable `y_2` by the ray equation (1):
// (5) `4x_2^2 + (s(x_2 - x_1) + y_1)^2 = 1`
//
// Since the the ellipse equation (2) is always 1 for each point, I can write:
// (6) `4x_2^2 + (s(x_2 - x_1) + y_1)^2 = 4x_1^2 + y_1^2`
//
// (7) `4x_2^2 + s^2(x_2 - x_1)^2 + 2s(x_2 - x_1)y_1 + y_1^2 = 4x_1^2 + y_1^2`
//
// (8) `4x_2^2 + s^2(x_2 - x_1)^2 + 2s(x_2 - x_1)y_1 = 4x_1^2`
//
// (9) `4(x_2^2 - x_1^2) + s^2(x_2 - x_1)^2 + 2s(x_2 - x_1)y_1 = 0`
//
// (10) `4(x_2 - x_1)(x_2 + x_1) + s^2(x_2 - x_1)^2 + 2s(x_2 - x_1)y_1 = 0`
//
// Dividing by the term `(x_2^2 - x_1^2)`:
// (11) `4(x_2 + x_1) + s^2(x_2 - x_1) + 2s y_1 = 0`
//
// (12) `4x_2 + 4x_1 + s^2 x_2 - s^2 x_1 + 2s y_1 = 0`
//
// (13) `4x_1 - s^2 x_1 + 2s y_1 = -4x_2 - s^2 x_2`
//
// (14) `4x_1 - s^2 x_1 + 2s y_1 = x_2 (-4 - s^2)`
//
// (15) `dfrac{4x_1 - s^2 x_1 + 2s y_1}{-4 - s^2} = x_2`
//
// Once I know `x_2` it's a walk in the park to get `y_2` with the help of (1).
//
// # Alternative
// There are several approaches to this problem:
// - you can use trigonometric equations instead of vector computations.
// - the intersection line/ellipse often leads to a quadratic equation which I believe is more prone to rounding errors
//
// # Note
// I even get the correct result when replacing ''double'' by ''float''. That's surprising.

#include <iostream>
#include <cmath>

// a simple 2D vector
struct Vector
{
  // create new vector
  Vector(double x_, double y_) : x(x_), y(y_) {}
  double x, y;

  // compute length
  double getLength() const
  {
    return sqrt(x*x + y*y);
  }
  // dot product
  double dot(const Vector& other) const
  {
    return x * other.x + y * other.y;
  }
};
// use a Vector for 2D points, too
typedef Vector Point;

int main()
{
  // count reflections
  unsigned int steps = 0;

  // initial ray
  Point from(0.0, 10.1);
  Point to  (1.4, -9.6);

  while (true)
  {
    // exit ellipse when x is between -0.01 and +0.01 on the upper side of the ellipse
    if (to.x >= -0.01 && to.x <= 0.01 && to.y > 10 - 0.1) // vertical radius is 10, subtract a bit because of curving
      break;

    // find inward pointing vector of the ellipse at intersection point
    Vector normal(-4 * to.x, -to.y);
    // normalize vector length
    double length = normal.getLength();
    normal.x /= length;
    normal.y /= length;

    // current direction of the ray
    Vector direction(to.x - from.x, to.y - from.y);
    // degenerated case ? ray will exit after only one bounce
    if (direction.x == 0)
    {
      steps++; // same as steps = 1 in this case
      break;
    }

    // reflect ray at intersection considering the normal
    // a short explanation can be found here: http://paulbourke.net/geometry/reflected/
    Vector reflect(0, 0);
    reflect.x = direction.x - 2 * direction.dot(normal) * normal.x;
    reflect.y = direction.y - 2 * direction.dot(normal) * normal.y;

    // slope of the reflected ray
    double slope = reflect.y / reflect.x;

    // previous endpoint becomes the start point of the next iteration
    from.x = to.x;
    from.y = to.y;

    // find next intersection
    // I derived that strange formula in the initial comment section
    to.x = (4*from.x - slope*slope*from.x + 2*slope*from.y) / (-4 - slope*slope);
    to.y = slope * (to.x - from.x) + from.y;

    // count iterations
    steps++;
  }

  // display result
  std::cout << steps << std::endl;
  return 0;
}
