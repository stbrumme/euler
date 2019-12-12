// ////////////////////////////////////////////////////////
// # Title
// Iterative Circle Packing
//
// # URL
// https://projecteuler.net/problem=199
// http://euler.stephan-brumme.com/199/
//
// # Problem
// Three circles of equal radius are placed inside a larger circle such that each pair of circles is tangent to one another and the inner circles do not overlap.
// There are four uncovered "gaps" which are to be filled iteratively with more tangent circles.
//
// ![circles](p199_circles_in_circles.gif)
//
// At each iteration, a maximally sized circle is placed in each gap, which creates more gaps for the next iteration.
// After 3 iterations (pictured), there are 108 gaps and the fraction of the area which is not covered by circles is 0.06790342, rounded to eight decimal places.
//
// What fraction of the area is not covered by circles after 10 iterations?
// Give your answer rounded to eight decimal places using the format x.xxxxxxxx .
//
// # Solved by
// Stephan Brumme
// September 2017
//
// # Algorithm
// Two days ago I published my solution to problem 510 where I mentioned that I got the final insights to solve 510 and 199 by going to a museum:
// by pure chance I saw some ancient drawings that looked pretty much like the circles in the example.
// And they had a name: Apollonian gasket (see https://en.wikipedia.org/wiki/Apollonian_gasket )
//
// The Wikipedia page linked to the Descartes' theorem (see https://en.wikipedia.org/wiki/Descartes%27_theorem ) which comes with a nice formula:
// (1) `(k_1 + k_2 + k_3 + k_4)^2 = 2(k_1^2 + k_2^2 +k_3^2 +k_4^2)`
//
// So when there are four circles touching each other the their "curvatures" `k` always fulfil that equation.
// The curvate is defined to be `k = 1/r` and, even more interesting, it can be negative when a circle touches the other with its inside.
// That's only the case for the outermost circle.
//
// If three curvatures are known then (1) can be transformed to compute the fourth:
// (2) `k_4 = k_1 + k_2 + k_3 \pm 2 sqrt{k_1 k_2 + k_2 k_3 + k_1 k_3}`
//
// I started with the three circles at level 1 and defined their radius to be 1. I found two circles: one between the three and one outside the three.
// The outermost circle (level 0) must be the one with the larger radius which turned out to be `3 - 2 sqrt{3} approx 2.1547`.
// Since it contains all other circle its curvature must be negative: `k = - dfrac{1}{3 - 2 sqrt{3}}`
//
// My function ''evaluate()'' recursively determines the the sum of the area covered by the "children" of three circles
// with curvatures ''k1'', ''k2'' and ''k3'' up to recursion level ''depth''.
// There are two distinct groups of "parents":
// - three V-shaped areas enclosed by the outermost circle and two circles at level 1
// - the small area between the three circles at level 1
//
// There is no need to compute each V-shaped area on its own: they are identical and I only need to compute one (and multiply by 3).
//
// Now I know the area of all children (up to level 10). I haven't added the area of the three circles at level 1 yet.
// Their sum divided by the area of the outermost circle gives the percentage of coverage, and 100% minus this value is the final result.
//
// # Note
// ''evalute()'' will often be called with the same parameter set and becomes pretty slow beyond level 16.
// I tried to add a small memoization scheme (same trick as always ...) but ended up slowing the the program for the default input 10 considerably.
// Therefore I removed it - even though it's more efficient for higher input values.

#include <iostream>
#include <cmath>

// return area of a circle
double getArea(double radius)
{
  return radius * radius * 3.14159265358979323846 / 4;
}

// return sum of touching circles' areas including their children
double evaluate(double k1, double k2, double k3, unsigned int depth)
{
  // https://en.wikipedia.org/wiki/Descartes%27_theorem
  auto k4 = k1 + k2 + k3 + 2 * sqrt(k1*k2 + k2*k3 + k1*k3);

  // area of the current circle
  auto radius = 1 / k4;
  auto area   = getArea(radius);
  // done ?
  if (depth == 1)
    return area;

  // keep going ...
  return area + evaluate(k1, k2, k4, depth - 1) +
                evaluate(k2, k3, k4, depth - 1) +
                evaluate(k1, k3, k4, depth - 1);
}

int main()
{
  // nesting level
  unsigned int depth = 10;
  std::cin >> depth;

  // the big circle can be computed with Descartes theorem
  // k1 = k2 = k3 = 1 and k4 = k1 + k2 + k3 + 2*sqrt(k1 + k2 + k3)
  // => k4 = 3 +- 2*sqrt(3)
  // case A: k4 = 3 + 2*sqrt(3) => radius approx. 0.1547 => that's the small circle between the three circles
  // case B: k4 = 3 - 2*sqrt(3) => radius approx. 2.1547 => that's the big circle I'm looking for
  auto outerK      = 3 - 2 * sqrt(3);
  auto outerRadius = -1 / outerK; // note: outerK is negative

  // the first three circle have radius = 1
  auto innerRadius = 1.0;
  auto innerK      = 1 / innerRadius;

  // (fully covered)  area of the first three circles
  auto initial = 3 * getArea(innerRadius);
  // (partially full) three identical V-shaped areas between two circles and the big circle
  auto vShaped = evaluate(outerK, innerK, innerK, depth);
  // (partially full) small area between the first three circles
  auto middle  = evaluate(innerK, innerK, innerK, depth);

  // total covered area
  auto result = initial + 3 * vShaped + middle;
  // as a percentage of the big circle
  result /= getArea(outerRadius);
  // empty = 100% - covered
  std::cout << 1 - result << std::endl;
  return 0;
}
