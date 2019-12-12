// ////////////////////////////////////////////////////////
// # Title
// Pythagorean Ant
//
// # URL
// https://projecteuler.net/problem=613
// http://euler.stephan-brumme.com/613/
//
// # Problem
// Dave is doing his homework on the balcony and, preparing a presentation about Pythagorean triangles,
// has just cut out a triangle with side lengths 30cm, 40cm and 50cm from some cardboard, when a gust of wind blows the triangle down into the garden.
// Another gust blows a small ant straight onto this triangle.
// The poor ant is completely disoriented and starts to crawl straight ahead in random direction in order to get back into the grass.
//
// Assuming that all possible positions of the ant within the triangle and all possible directions of moving on are equiprobable,
// what is the probability that the ant leaves the triangle along its longest side?
// Give your answer rounded to 10 digits after the decimal point.
//
// # Solved by
// Stephan Brumme
// November 2017
//
// # Algorithm
// The side lengths 30, 40 and 50 sound familiar: yes, it's a triangle with a right angle.
// Since the size of the triangle doesn't affect the way the ant can move, I replaced it by a simpler 3,4,5 triangle.
//
// Furthermore, let's assume that the triangles landed in such a way that
// - the side `a=3` aligned with the x-axis of a virtual coordinate system.
// - the side `b=4` aligned with the y-axis of a virtual coordinate system.
// - the ant is at point `P`
//
// So it looks like this:
//
// <![CDATA[
//  <svg width="10em" height="10em">
//   <line x1= "1%" y1= "1%" x2= "1%" y2="99%" stroke="black" stroke-width="1" />
//   <line x1= "1%" y1="99%" x2="75%" y2="99%" stroke="black" stroke-width="1" />
//   <line x1= "1%" y1= "1%" x2="75%" y2="99%" stroke="black" stroke-width="1" />
//   <text x= "3%" y="50%" style="font-size: 1em">a</text>
//   <text x="30%" y="97%" style="font-size: 1em">b</text>
//   <text x="30%" y="55%" style="font-size: 1em">c</text>
//
//   <text x="20%" y="75%" style="font-size: 1em" fill="red">P</text>
//  </svg>
// ]]>
//
// If I know the coordinates of `P` then I can compute the probability the the ant exits along the side `c`:
// if the ant chooses any direction within the angle `gamma` then that probabiltiy is `gamma / 2 pi`
//
// <![CDATA[
//  <svg width="10em" height="10em">
//   <line x1= "1%" y1= "1%" x2= "1%" y2="99%" stroke="black" stroke-width="1" />
//   <line x1= "1%" y1="99%" x2="75%" y2="99%" stroke="black" stroke-width="1" />
//   <line x1= "1%" y1= "1%" x2="75%" y2="99%" stroke="black" stroke-width="1" />
//
//   <line x1= "1%" y1= "1%" x2="20%" y2="75%" stroke="red" stroke-width="1" />
//   <line x1="75%" y1="99%" x2="20%" y2="75%" stroke="red" stroke-width="1" />
//   <text x="20%" y="75%" style="font-size: 1em" fill="red">P</text>
//
//   <line x1= "1%" y1="75%" x2="20%" y2="75%" stroke="gray" stroke-width="1" />
//   <line x1="20%" y1="99%" x2="20%" y2="75%" stroke="gray" stroke-width="1" />
//   <text x= "1%" y="25%" style="font-size: 1em" fill="gray">&alpha;</text>
//   <text x="55%" y="97%" style="font-size: 1em" fill="gray">&beta;</text>
//   <text x="30%" y="70%" style="font-size: 1em" fill="gray">&gamma;</text>
//  </svg>
// ]]>
//
// The angles `alpha` and `beta` can be derived from the coordinates `P(x,y)`:
// (1) `tan(alpha) = dfrac{x}{b - y}`
// (2) `tan(beta)  = dfrac{y}{a - x}`
//
// Solving for `alpha` and `beta`:
// (3) `alpha = arctan(dfrac{x}{b - y})`
// (4) `beta  = arctan(dfrac{y}{a - x})`
//
// That's exactly what the functions ''getAlpha'' and ''getBeta'' do.
//
// The third angle `gamma` is based solely on `alpha` and `beta`:
// - the sum of all angles around the point `P` is always `2 pi` (=`360^{\circ}`)
// - all angles in any triangle sum to `pi`
// - a right angle equals `frac{pi}{2}`
// - so the angle "below"               `P` is `pi - frac{pi}{2} - alpha = frac{pi}{2} - alpha`
// - so the angle "on the left side" of `P` is `pi - frac{pi}{2} - beta  = frac{pi}{2} - beta`
// Therefore
// (5) `2 pi = gamma + (frac{pi}{2} - alpha) + (frac{pi}{2} - beta) + frac{pi}{2}`
// (6) `gamma = 2 pi - (frac{pi}{2} - alpha) - (frac{pi}{2} - beta) - frac{pi}{2}`
// (7) `gamma = frac{pi}{2} + alpha + beta`
//
// And the probability is (as mentioned before):
// (8) `f(x,y) = dfrac{gamma}{2 pi}`
//
// (9) `f(x,y) = dfrac{1}{2 pi} * (dfrac{pi}{2} + arctan(dfrac{x}{b - y}) + arctan(dfrac{y}{a - x}))`
//
// The average value `f(x,y)` of all points `(x,y)` on the triangle's surface will be the answer to the problem.
//
// However, I couldn't find a fast and reliable way to do it in C++ (without external libraries).
// The code you see below is good for the first four digits - but I need to provide ten accurate digits.
//
// That's when I started playing around with Wolfram Alpha:
// - the integral `\int{\int{f(x,y)}dx}dy` divided by the triangle's area `dfrac{3 * 4}{2} = 6` is the exact answer
//
// - and Wolfram Alpha simplified that to `0.5 + log((2^16 3^4 sqrt{3/5})/5^12) / 12 pi` - I would have never found that formula !
//
// - evaluating the equation gives the correct result, even in C++ ...
//
// # Note
// I always strive to put all tools needed to solve a problem into a single C++ file.
// This time my C++ code is basically useless because an external tool (Wolfram Alpha) condensed the whole solution to a single equation.
// Therefore I don't like this problem at all - I found the right approach to the problem and just couldn't take the last step because
// it required wizardry that is wildly outside my capabilities.
// Reading the forums it appears that 95% of all solvers walked the same path and relied on Mathematica or similar tools.

#include <iostream>
#include <iomanip>
#include <cmath>

// triangle (same shape as 30,40,50 !)
const double a = 3; // shortest leg
const double b = 4; // longest  leg
const double c = 5; // hypotenuse

// |\.
// | \.
// | \.
// b  c
// |   \.
// |   \.
// +--a-\.

const double PI = 3.1415926535897932384626433832795028;

// compute angle in upper left corner
double getAlpha(double x, double y)
{
  // tan(alpha) = opposite / adjacent = X / Y
  // alpha      = arctan(X / Y)
  // where X = x and Y = b - y
  return atan(x / (b - y));
}

// compute angle in lower right corner
double getBeta(double x, double y)
{
  // similar to getAlpha:
  // tan(beta) = opposite / adjacent = Y / X
  // beta      = arctan(Y / X)
  // where X = a - x and Y = y
  return atan(y / (a - x));
}

// return probability of leaving the triangle
double getProbability(double x, double y)
{
  // the of all angles in a triangle is pi:
  // pi =         otherAlpha +        otherBeta + gamma
  // but the big triangle must fulfil the same condition, so
  // pi = alpha + otherAlpha + beta + otherBeta + pi/2
  // therefore the difference is
  // 0  = gamma - alpha - beta - pi/2
  // gamme = alpha + beta + pi/2
  auto gamma = PI/2 + getAlpha(x, y) + getBeta(x, y);
  // each direction is equally likely: "exit probability" = gamma / full circle
  // and a full circle has 360 degrees => 2pi
  return gamma / (2 * PI);
}

int main()
{
  // display 10 digits
  std::cout << std::fixed << std::setprecision(10);

  double epsilon = 0.01;
  //std::cin >> epsilon;

  // add all percentages
  double sum = 0;
  unsigned long long numPoints = 0;
  for (double x = epsilon; x < a; x += epsilon)
  {
    auto height = b - x * b/a;
    for (double y = epsilon; y < height; y += epsilon)
    {
      auto current = getProbability(x, y);
      sum += current;

      numPoints++;
      //if (numPoints % 10000000 == 0)
        //std::cout << numPoints << std::endl;
    }
  }

  // compute average percentage
  auto result = sum / numPoints;
  // ==> uncomment the following line to see my approximate result
  //std::cout << result << std::endl;

  // I reduced the triangle's dimensions from 30,40,50 to 3,4,5 (doesn't affect the probability)
  // and Wolfram Alpha double-integrated to
  // 0.5 + log((2^16 3^4 sqrt(3/5))/5^12) / 12pi
  // (actually Wolfram Alpha's output contained large numbers: 5308416 = 2^16 3^4 and 244140625 = 5^12)
  // I have no clue how Wolfram Alpha found that strange equation
  // but the resulting value matched my approximation and it's indeed correct
  result = 0.5 + log((pow(2, 16) * pow(3, 4) * sqrt(3.0 / 5)) / pow(5, 12)) / (12*PI);
  // and here comes the accurate result
  std::cout << result << std::endl;
  return 0;
}
