// ////////////////////////////////////////////////////////
// # Title
// An Arithmetic Geometric sequence
//
// # URL
// https://projecteuler.net/problem=235
// http://euler.stephan-brumme.com/235/
//
// # Problem
// Given is the arithmetic-geometric sequence `u(k) = (900-3k)r^{k-1}`.
// Let `s(n) = sum_{k=1...n}u(k)`.
//
// Find the value of `r` for which `s(5000) = -600,000,000,000`.
//
// Give your answer rounded to 12 places behind the decimal point.
//
// # Solved by
// Stephan Brumme
// July 2017
//
// # Algorithm
// The function `s_r(5000)` is monotonically decreasing: larger `r` reduce `s_r(5000)`.
//
// My program was inspired by the a classic bisection algorithm ( https://en.wikipedia.org/wiki/Bisection_method ):
// 1. start with two reasonable upper and lower limits (I choose 0 and 2)
// 2. find `mid = frac{upper+lower}{2}` and `s_{mid}(5000)`
// 3. if the result is less than -600 billion then `mid` is too large and set `upper = mid`
// 4. if the result is greater than -600 billion then `mid` is too small and set `lower = mid`
// 5. if `upper - lower > 10^-12` then go to step 2.
//
// The ''pow'' function can be pretty slow. Incrementally computing `r^k` is about ten times faster (see ''#define SLOW'').
//
// # Note
// I actually stop only after `upper - lower < 10^-13` because I was afraid of rounding issues.

#include <iostream>
#include <iomanip>
#include <cmath>

//#define SLOW

// compute s(r) according to problem statement
double s(double r)
{
  double result = 0;
  double x = 1; // r^0 is always 0
  for (int k = 1; k <= 5000; k++)
  {
#ifdef SLOW
    result += (900 - 3 * k) * pow(r, k - 1);
#else
    result += (900 - 3 * k) * x;
    x *= r;
#endif
  }
  return result;
}

int main()
{
  // initial lower and upper bounds
  double lower = 0;
  double upper = 2;

  // until the range is small enough
  while (upper - lower > 0.0000000000001)
  {
    double mid = (upper + lower) / 2;
    // find result at midpoint
    double current = s(mid);

    // adjust borders
    if (current < -600000000000.0)
      upper = mid;
    else
      lower = mid;
  }

  // print result (use midpoint but upper and/or lower should yield the same output)
  std::cout << std::fixed << std::setprecision(12) << (upper + lower) / 2 << std::endl;
  return 0;
}
