// ////////////////////////////////////////////////////////
// # Title
// Concave triangle
//
// # URL
// https://projecteuler.net/problem=587
// http://euler.stephan-brumme.com/587/
//
// # Problem
// A square is drawn around a circle as shown in the diagram below on the left.
// We shall call the blue shaded region the L-section.
// A line is drawn from the bottom left of the square to the top right as shown in the diagram on the right.
// We shall call the orange shaded region a concave triangle.
//
// ![L shape](p587_concave_triangle_1.png)
//
// It should be clear that the concave triangle occupies exactly half of the L-section.
//
// Two circles are placed next to each other horizontally, a rectangle is drawn around both circles, and a line is drawn from the bottom left to the top right as shown in the diagram below.
//
// ![multiple circles](p587_concave_triangle_2.png)
//
// This time the concave triangle occupies approximately 36.46% of the L-section.
//
// If n circles are placed next to each other horizontally, a rectangle is drawn around the n circles, and a line is drawn from the bottom left to the top right, then it can be shown that the least value of n for which the concave triangle occupies less than 10% of the L-section is n = 15.
//
// What is the least value of n for which the concave triangle occupies less than 0.1% of the L-section?
//
// # Solved by
// Stephan Brumme
// October 2017
//
// # Algorithm
// This is a so-called paper'n'pen problem because you can solve it without a computer.
// But I like computers so much that I decided to write a proper program that solves this problem by throwing CPU cycles at it !
//
// A little mathematics is still required:
// - the line from the lower left corner to the upper right corner has the equation `y = mx` where `m` is the slope
// - I define that my circles have a radius `r = 1`
// - so that the box has a height of `r+r = 2`
//
// If there is only one circle then it covers an area of `pi r^2 = pi 1^2 = pi`. Its bounding box covers an area of `2^2 = 4`.
// There are four L-shaped areas between circle and box - all have the same size, so that one L-shaped area covers an area of
// (1) `(4 - pi) / 4 approx 0.2146`
//
// To find the intersection between the line and the first circle - which is centered at (1,1) - I solve both equation for `y`:
// (2) `y = mx`
// (3) `(x - 1)^2 + (y - 1)^2 = 1`
// (4) `(y - 1)^2 = 1 - (x - 1)^2`
// (5) `y - 1 = \pm sqrt{1 - (x - 1)^2}`
// (6) `y = 1 - sqrt{1 - (x - 1)^2}`  <== I only need the lower half of the circle where the first intersection takes place
//
// The intersection is at (2) = (6):
// (7) `mx = 1 - sqrt{1 - (x - 1)^2}`
//
// And that's when I decided to write a "true" IT solution and solve equation (7) numerically:
// - compute left and right side of equation (7)
// - if they are "close enough", then abort
// - else move `x` accordingly
// - start with large moves and then make smaller moves
// - see ''getIntersection()'' (where ''slope'' is ''m'')
//
// Knowing the intersection I proceed with the same motivation: keep solving it numerically !
// - function ''getAreaL()'' computes the area of the L-shaped area given a ''slope''
// - left  of the intersection point is a triangle with area `x * y / 2 = x * m x / 2`
// - right of the intersection point is the integral of equation (6)
// - my numerical integration takes 100000 steps between the intersection point and `x = 1`
//
// At first I checked every possible size of 1,2,3,...,100,... circles whether the area is below 0.1%.
// It took a little more than one second to find the correct solution.
// Knowing that the area is strictly decreasing I re-wrote my code to work similar to binary intersection:
// check only every 64th number: 64, 128, 192, ... circles and when the area is below the limit then go back and check every 32nd, then 16th, ... until ''step = 1''.
// Now the result is printed after less than 0.1 seconds.
//
// # Note
// Of course there are a few assumptions which are not always true:
// - maybe ''Epsilon'' is too large and the result will be wrong
// - maybe I need more than 100000 steps to accurately evaluate ''getAreaL''
// ==> but my default values easily solve the problem
//
// I had fun writing this solution - for whatever reason I like numerical approaches such as Monte-Carlo simulations.
//
// # Alternative
// As I told you before, you can solve this with pen'n'paper (or [Wolfram Alpha](http://www.wolframalpha.com) ).

#include <iostream>
#include <cmath>

const auto Epsilon = 0.00000000001;
const auto NoLine  = 0;

// return x where line and first circle intersection
double getIntersection(double slope)
{
  // circle: (x-1)^2 + (y-1)^2 = 1
  //                   (y-1)^2 = 1 - (x-1)^2
  //                    y-1    = sqrt(1 - (x-1)^2)
  //                         y = 1 - sqrt(1 - (x-1)^2)
  // line:                   y = mx    <= where m is the slope
  // their intersection:    mx = 1 - sqrt(1 - (x-1)^2)

  // I entered those formula in Wolfram Alpha and didn't like the output ... so let's solve this numerically !
  // choose any x in 0 < x < 1
  auto x    = 0.5;
  // adjust x by this value if not precise enough
  auto step = 0.1;

  while (true)
  {
    // good enough ?
    auto leftSide  = slope * x;
    auto rightSide = 1 - sqrt(1 - (x - 1)*(x - 1)); // see above how I got this formula
    if (fabs(leftSide - rightSide) < Epsilon)
      return x;

    // move x
    if (leftSide > rightSide)
      x -= step;
    else
      x += step;

    // take smaller steps
    step *= 0.99;
  }
}

// return area of the L-shaped area
double getAreaL(double slope)
{
  // x-coordinate of first intersection between line and circle
  double intersection = 0;
  // area of the triangle on the left side of the intersection
  double leftArea     = 0;
  if (slope > 0)
  {
    intersection = getIntersection(slope);
    leftArea = intersection * (intersection * slope) / 2;
  }

  // the area on the right side of the intersection can be computed exactly - but I hate ugly looking formulas and prefer C++ code :-)

  // numerically integrate area between intersection and "where the first circle touches the box"
  auto rightArea = 0.0;
  auto Step = (1 - intersection) / 100000;
  for (double x = intersection; x < 1; x += Step)
  {
    double y = 1 - sqrt(1 - (x - 1)*(x - 1)); // same formula as in getIntersection
    rightArea += y * Step;
  }

  return leftArea + rightArea;
}

int main()
{
  // stop if below 0.1%
  auto limit = 0.1;
  std::cin >> limit;

  // a circle has an area of A=pi * r^2
  // r=1 => A = pi
  // the box around the circle has an area of 2^2 = 4
  // there are four L's between circle and box, I only need the one in the lower half
  // L = (4 - pi) / 4 = 0.21460183660255169038433915418012...
  auto L = getAreaL(NoLine);   // NoLine=0 is a special case in getAreaL

  auto numCircles = 1;
  auto step = 64; // powers of two are great, but could be any other positive number, too (even 1)
  while (true)
  {
    auto slope = 1.0 / numCircles;
    auto area  = getAreaL(slope);
    // area small enough ?
    auto percentage = 100 * area / L;
    if (percentage < limit)
    {
      // reached small step size, found solution
      if (step == 1)
        break;

      // maybe I "jumped over" the correct solution, go back and reduce step size
      numCircles -= step;
      step /= 2;
    }

    // look at next slope
    numCircles += step;
  }

  // and print the solution
  std::cout << numCircles << std::endl;
  return 0;
}
