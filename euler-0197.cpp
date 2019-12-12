// ////////////////////////////////////////////////////////
// # Title
// Investigating the behaviour of a recursively defined sequence
//
// # URL
// https://projecteuler.net/problem=197
// http://euler.stephan-brumme.com/197/
//
// # Problem
// Given is the function `f(x) = \lfloor 2^{30.403243784-x^2} \rfloor * 10^-9` ( `\lfloor \space \rfloor` is the floor-function),
// the sequence `u_n` is defined by `u_0 = -1` and `u_{n+1} = f(u_n)`.
//
// Find `u_n + u_{n+1}` for `n = 10^12`.
// Give your answer with 9 digits after the decimal point.
//
// # Solved by
// Stephan Brumme
// June 2017
//
// # Algorithm
// The function ''f(x)'' is a straightforward implementation of the formula specified in the problem statement.
//
// I printed the first results "to get a feeling" for the value. It's obvious that they quickly converge to a bi-stabil state.
// Assuming that this doesn't change, I computed `f(u_1000) + f(u_1001)` and hoped for the best ... and yes, got the correct answer.
//
// Actually you always get the correct answer after about 500 steps: `u_512 + u_513` are the smallest on my machine.
// Depending on some rounding issues it's possible that you need more or less steps on your computer.

#include <iostream>
#include <iomanip>
#include <cmath>

// compute next step
double f(double x)
{
  return floor(pow(2.0, 30.403243784 - x*x)) * pow(10, -9);
}

int main()
{
  // initial value
  double u = -1;

  // compute next steps
  double next = f(u);
  for (unsigned int i = 1; i < 513; i++)
  {
    u = next;
    next = f(u);
  }

  // sum of two neighbors
  double result = u + next;

  std::cout << std::fixed << std::setprecision(9) << result << std::endl;
  return 0;
}
