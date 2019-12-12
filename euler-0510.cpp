// ////////////////////////////////////////////////////////
// # Title
// Tangent Circles
//
// # URL
// https://projecteuler.net/problem=510
// http://euler.stephan-brumme.com/510/
//
// # Problem
// Circles A and B are tangent to each other and to line L at three distinct points.
// Circle C is inside the space between A, B and L, and tangent to all three.
// Let `r_A`, `r_B` and `r_C` be the radii of A, B and C respectively.
//
// ![circles](p510_tangent_circles.png)
//
// Let `S(n) = sum{r_A + r_B + r_C}`, for `0 < r_A <= r_B <= n` where `r_A`, `r_B` and `r_C` are integers.
// The only solution for `0 < r_A <= r_B <= 5` is `r_A = 4`, `r_B = 4` and `r_C = 1`, so `S(5) = 4 + 4 + 1 = 9`.
// You are also given `S(100) = 3072`.
//
// Find `S(10^9)`.
//
// # Solved by
// Stephan Brumme
// September 2017
//
// # Algorithm
// A simple ''bruteForce()'' function easily verifies `S(100)` (and even manages to solve `S(1000)`)
// but then falls apart due to rounding problems and - more import - performance issues.
// However, it reveals a few things:
// - most solutions are multiples of (4,4,1)
// - and in general: for each solution all of its multiples are solutions, too
// - if I remove these multiples, then the basic solutions are squares only
//
// Just a few days I was in a museum and saw some ancient drawings which helped me to get great insights into problem 199
// (which is not really solved yet: I still have some severe performance issues but they aren't related to the geometric solution).
// Anyway, I learnt that these kind of problems have been around for ages and were analyzed by Appolonius more than 2000 years ago (see https://en.wikipedia.org/wiki/Problem_of_Apollonius ).
// Ren&egrave; Descartes found a great equation (see https://en.wikipedia.org/wiki/Descartes%27_theorem ) if four circles touch each other and their curvature is `k = 1 / r`
// (1) `(k_1 + k_2 + k_3 + k_4)^2 = 2 (k_1^2 + k_2^2 + k_3^2 + k_4^2)`
//
// Unfortunately, I don't have four but three circles and a line ...
// and fortunately, if I set `k = 0` then the formula solves the case of three circles and a line, too !
//
// Now the whole equation simplifies to
// (2) `(k_A + k_B + k_C)^2 = 2 (k_A^2 + k_B^2 + k_C^2)`
// If I solve for `k_C` then
// (3) `sqrt{k_C} = sqrt{k_A} + sqrt{k_B}`
//
// And with regards to the radii instead of the curvatures (remember: `k = 1 / r`)
// (4) `dfrac{1}{sqrt{r_C}} = dfrac{1}{sqrt{r_A}} + dfrac{1}{sqrt{r_B}}`
//
// As I said before, all solutions are squares (or multiples of squares).
// If I assume `a = sqrt{r_A}` and `b = sqrt{r_B}` and `c = sqrt{r_C}` then:
//
// (5) `dfrac{1}{c} = dfrac{1}{a} + dfrac{1}{b}`
//
// Solved for `c`
// (6) `c = dfrac{ab}{a+b}`
//
// My program iterates over all `a` and `b` where `b < a` (that's different from the problem statement but it doesn't affect the result).
// If `a^2 b^2` is a multiple of `a^2 + b^2` then I have found another `c^2`.
// That means I have found a triple `(a^2, b^2, c^2) = (r_A, r_B, r_C)`.
//
// It's possible that the multiples of squares are squares, too.
// Therefore I have to make sure that they are coprime (`gcd(a,b,c) = 1`) otherwise they would be counted twice.
//
// The sum of all multiples of `r_A + r_B + r_C` is based on the triangular numbers (see ''triangle()'').
// If `m` is the number of multiples of a basic solution:
// (7) `m = dfrac{limit}{r_A}`     ==> because I defined `r_A >= r_B`
//
// (8) `sum_{k=1..m}{k} = dfrac{m(m+1)}{2}`
//
// (9) `sum_{k=1..m}{k (r_A + r_B + r_C)} = (r_A + r_B + r_C) dfrac{m(m+1)}{2}`
//
// # Alternative
// After submitting my solution I saw in the Project Euler forum that you can heavily re-arrange the ''for''-loops such that the solution is found in a few milliseconds.

#include <iostream>
#include <cmath>

// ---------- taken from my toolbox ----------

// greatest common divisor
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

// ---------- brute force solution (not used anymore) ----------

// try all combinations of a,b,c
unsigned int bruteForce(unsigned int limit)
{
  // accept solution if left side of the equation of Descartes theorem
  // doesn't differ by more than epsilon from the right side
  const double Epsilon = 0.0000000000001;

  // sum of all valid a,b,c
  unsigned int result = 0;
  for (unsigned int a = 1; a <= limit; a++)
    for (unsigned int b = 1; b <= a; b++)
      for (unsigned int c = 1; c <= b; c++)
      {
        // curvatures
        double k1 = 1.0 / a;
        double k2 = 1.0 / b;
        double k3 = 1.0 / c;
        // both sides of the equation
        double leftSide  = (k1 + k2 + k3) * (k1 + k2 + k3);
        double rightSide = 2 * (k1*k1 + k2*k2 + k3*k3);
        // acceptable error ?
        if (fabs(leftSide - rightSide) <  Epsilon)
        {
          auto sum = a + b + c;
          //std::cout << a << "+" << b << "+" << c << "=" << sum << std::endl;
          result += sum;
        }
      }

  return result;
}

// ---------- relevant code ----------

// compute triangular number
unsigned long long triangle(unsigned int n)
{
  // sum of 1+2+3+...+n
  // see https://en.wikipedia.org/wiki/Triangular_number
  unsigned long long nn = n;
  return nn * (nn + 1) / 2;
}

typedef long long Number;
unsigned long long evaluate(unsigned int limit)
{
  // all multiples of the basic 4+4+1 solution
  unsigned long long result = 0;

  // all multiples of a^2, b^2, c^2
  // where 1/c2 = (1/a2 + 1/b2) + 2sqrt(1/a2 * 1/b2)
  // => sqrt(1/c2) = sqrt(1/a2) + sqrt(1/b2)
  // => 1/c = 1/a + 1/b
  // => c = a*b / (a+b)
  for (unsigned long long a = 1; a*a < limit; a++)
    for (unsigned long long b = 1; b <= a; b++)
    {
      // numerator = a*a * b*b
      auto a2 = a*a;
      auto b2 = b*b;
      auto numerator = a2 * b2;

      // denominator = (a+b)^2
      auto denominator = (a+b) * (a+b);

      // c^2 must be an integer
      auto c2 = numerator / denominator;
      // no remainder left by the previous divison ?
      if (c2 * denominator != numerator)
        continue;

      // all values must be coprime
      if (gcd(gcd(a2, b2), c2) == 1)
      {
        auto multiples = limit / (unsigned int)a2;
        auto sum = (a2 + b2 + c2) * triangle(multiples);
        result += sum;
      }
    }

  return result;
}

int main()
{
  unsigned int limit = 1000000000;
  std::cin >> limit;

  //std::cout << bruteForce(limit) << std::endl;
  std::cout << evaluate  (limit) << std::endl;
  return 0;
}
