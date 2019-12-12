// ////////////////////////////////////////////////////////
// # Title
// A Scoop of Blancmange
//
// # URL
// https://projecteuler.net/problem=226
// http://euler.stephan-brumme.com/226/
//
// # Problem
// The blancmange curve is the set of points `(x,y)` such that `0 <= x <= 1` and `y = sum_{n=0}^{infinity} dfrac{s(2^n x)}{2^n}`,
// where `s(x)` is the distance from `x` to the nearest integer.
//
// The area under the blancmange curve is equal to `\frac12` shown in pink in the diagram below.
// ![blancmange curve](p226_scoop2.gif)
//
// Let `C` be the circle with centre `(\frac14, \frac12)` and radius `\frac14`, shown in black in the diagram.
//
// What area under the blancmange curve is enclosed by `C`?
// Give your answer rounded to eight decimal places in the form 0.abcdefgh
//
// # Solved by
// Stephan Brumme
// September 2017
//
// # Algorithm
// My idea is:
// - find the left and right intersection of the circle with the blancmange curve
// - numerically integrate the area between those limits
//
// I saw only two intersections in the image and boldly assumed that there exist only those two intersections.
// The right intersection seems to be pretty close to the centre of the blancmange.
//
// The function ''S(x)'' evaluates the given formula: it runs several iterations for ''n = 0,1,2,...''.
// The denominator grows much faster than the numerator and when a term is below my accuracy threshold ''Epsilon'' then I stop and return the sum.
//
// If `(x_b, y_b)` represent a point on the blancmange curve and `(x_c, y_c)` a point on the circle then each intersection is a solution of
// (1) `d_x = | x_b - x_c |` and `d_y = | y_b - y_c |` and radius `r`
// (2) `r^2 = d_x^2 + d_y^2`
//
// ''findIntersection()'' calls ''S()'' a lot: it slowly walks along the curve `y_b = S(x_b)` until it is too far from the circle.
// Then it turns around and walks towards the circle's center but with 50% of the previous step size.
// There are four different cases to consider which make my function look ugly and I suspect this can be done a lot better.
// Nevertheless, the two intersections are found pretty fast (and the right one is indeed at `x = 0.5`).
//
// In order to numerically integrate the area shared by circle and blancmange curve I have to compute
// (3) `integral_{x_1}^{x^2} y_b - y_c`
// where `x_1` and `x_2` are the intersections (I call them ''from'' and ''to'' in my code).
//
// Integrating the circle needs a bit of tweaking: for a circle centered at the origin I can say
// (4) `r^2 = x^2 + y^2`
// (5) `y = sqrt{r^2 - x^2}`
//
// There are two solutions: the "upper" and the "lower" half of the circle. From the image it's obvious that only the lower half is of interest.
// Shifting the circle to an arbitrary position changes (5) to
// (6) `y_c - y = sqrt{r^2 - (x_c - x)^2}`
// (7) `y = y_c - sqrt{r^2 - (x - x_c)^2}`
//
// My program subtracts (7) from (3). I found that a step size of 0.00001 produces the correct result.
// A larger step size has rounding errors while smaller step sizes are much slower.
//
// # Note
// I started with incredibly small values of ''Epsilon'' and ''step'' because I suspected a heavy influence of rounding errors.
// After a short debugging session I saw that much larger values still produce the correct result.
//
// Maybe my thresholds are too tightly related to the default values of the problem such that the live test isn't correct in all cases (last digits could be "garbage").

#include <iostream>
#include <iomanip>
#include <cmath>

// smallest error threshold that still produces the correct output
const double Epsilon = 0.00000001;

// compute S(x) for the blancmange curve
double S(double x)
{
  // https://en.wikipedia.org/wiki/Blancmange_curve
  double result = 0;
  for (unsigned int n = 0; ; n++) // no abort-condition, exit when error is below epsilon threshold
  {
    auto power = pow(2, n);
    auto parameterS = power * x;

    // distance to smaller integer
    auto s = parameterS - floor(parameterS);
    // if > 0.5 then the bigger integer is closer
    if (s > 0.5)
      s = 1 - s;

    auto add = s / power;
    result += add;

    // enough precision ?
    if (add < Epsilon)
      return result;
  }
}

// find intersection of S and the circle by scanning x with increment step, abort if precision exceeds epsilon
double findIntersection(double circleX, double circleY, double radius, double x, double step)
{
  while (true)
  {
    // current position on the blancmange curve
    auto y = S(x);

    // distance to the circle's center
    auto deltaX = x - circleX;
    auto deltaY = y - circleY;
    auto distance = sqrt(deltaX * deltaX + deltaY * deltaY);

    // when distance = radius, then I'm finished (allow a certain error margin Epsilon)
    auto error = fabs(distance - radius);
    if (error < Epsilon)
      return x;

    // will be true if I'm going in the wrong direction:
    // then turn around and decrease the step size
    bool turnAround = false;

    if (distance < radius) // keep "going away" from the circle's center
    {
      // to the right of the circle's center
      if (deltaX > 0 && step < 0)
        turnAround = true;
      // to the left of the circle's center
      if (deltaX < 0 && step > 0)
        turnAround = true;
    }
    else                   // I'm too far from the circle, need to "come back"
    {
      // to the right of the circle's center
      if (deltaX > 0 && step > 0)
        turnAround = true;
      // to the left of the circle's center
      if (deltaX < 0 && step < 0)
        turnAround = true;
    }

    // switch direction, smaller steps
    if (turnAround)
      step = -step / 2;

    // next step
    x += step;
  }
}

// numerically integrate area inside the circle
double integrate(double circleX, double circleY, double radius, double from, double to, double step)
{
  double result = 0;
  for (auto x = from; x <= to; x += step)
  {
    auto upper = S(x);
    // basic circle is: r^2 = x^2 + y^2
    // solved for y:      y = sqrt(r^2 - x^2)
    // I can ignore the second solution which belongs to the "upper" part of the circle
    // however, the current circle isn't centered at the origin, therefore
    //                    y = circleY - sqrt(r^2 - (x - circleX)^2)
    auto lower = circleY - sqrt(radius*radius - (x - circleX)*(x - circleX));

    // add area
    result += (upper - lower) * step;
  }
  return result;
}

int main()
{
  auto circleX = 0.25;
  auto circleY = 0.5;
  auto radius  = 0.25;
  std::cin >> circleX >> circleY >> radius;

  std::cout << std::fixed << std::setprecision(8);

  // find intersections
  auto from = findIntersection(circleX, circleY, radius, circleX, -0.1);
  auto to   = findIntersection(circleX, circleY, radius, circleX, +0.1);

  // start numerical integration
  auto step = 0.00001;
  auto area = integrate(circleX, circleY, radius, from, to, step);

  std::cout << area << std::endl;
  return 0;
}
