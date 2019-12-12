// ////////////////////////////////////////////////////////
// # Title
// Marsh Crossing
//
// # URL
// https://projecteuler.net/problem=607
// http://euler.stephan-brumme.com/607/
//
// # Problem
// Frodo and Sam need to travel 100 leagues due East from point A to point B. On normal terrain, they can cover 10 leagues per day, and so the journey would take 10 days.
// However, their path is crossed by a long marsh which runs exactly South-West to North-East, and walking through the marsh will slow them down.
// The marsh is 50 leagues wide at all points, and the mid-point of AB is located in the middle of the marsh.
//
// A map of the region is shown in the diagram below:
//
// ![map](p607_marsh.png)
//
// The marsh consists of 5 distinct regions, each 10 leagues across, as shown by the shading in the map.
// The strip closest to point A is relatively light marsh, and can be crossed at a speed of 9 leagues per day.
// However, each strip becomes progressively harder to navigate, the speeds going down to 8, 7, 6 and finally 5 leagues per day for the final region of marsh,
// before it ends and the terrain becomes easier again, with the speed going back to 10 leagues per day.
//
// If Frodo and Sam were to head directly East for point B, they would travel exactly 100 leagues, and the journey would take approximately 13.4738 days.
// However, this time can be shortened if they deviate from the direct path.
//
// Find the shortest possible time required to travel from point A to B, and give your answer in days, rounded to 10 decimal places.
//
// # Solved by
// Stephan Brumme
// October 2017
//
// # Algorithm
// Each section of the marsh is leagues wide. However, walking the direct route means they walk `sqrt{2} * 10 approx 14.1421` leagues in each section.
// In order to be faster they should walk less than 14.1421 leagues in each slow section and walk a longer distance on the normal terrain.
//
// The whole map is a two-dimensional coordinate system. Initially I tried to have city A at `(0,0)` and B at `(100,0)` but things got messy.
// Then I decided to rotate the whole map by 45 degrees such that the marsh is parallel to the y-axis.
// City A still remains at `(0,0)` while B is located at `(dfrac{100}{sqrt{2}}, dfrac{100}{sqrt{2}})`.
// The distance between both cities is still 100 leagues.
//
// Knowing that the marsh is exactly 50 leagues wide, the marsh' smallest `x` and largest `x` are 50 units apart.
// Since `x_B - x_A = dfrac{100}{sqrt{2}} - 0 approx 70.7107` the marsh starts at `x_1 approx (70.7107 - 50) / 2 approx 10.3553`.
// The next crossing will be at `x_2 = x_1 + 10`, then `x_3 = x_2 + 10`, ..., `x_6 = x_5 + 10 = x_1 + 50`.
//
// The direct route is `x = y` and will be stored in my container named ''points''.
// The function ''duration()'' determines the distance between consecutive points and divide them by the speed of Frodo and Sam (see ''speed'').
// It returns ''13.4738023615'' for the direct route.
//
// I like Monte-Carlo simulations because even though they seem to be unpredictable they produce a predictable result after some time.
// This problem can be solved by randomizing the y-components of the marsh:
// - ''mutate'' picks a random point out of ''points'', except for the first and last (A and B)
// - it randomly adds or subtracts a given ''delta''
// - if ''duration()'' returns a shorter time than before then keep this mutation, else undo it
//
// My program starts with a large ''delta'' and successively reduces it.
// The way I reduce ''delta'' and how often I call ''mutate()'' is not scientific at all:
// I started with many more iterations and a slower decrease but saw that the coarser values still produce the correct result.
//
// # Alternative
// The Gradient Descent method is a more straightforward optimization method (see https://en.wikipedia.org/wiki/Gradient_descent ).
// The problem can be solved with Snell's law, too (see https://en.wikipedia.org/wiki/Snell%27s_law ).

#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>

// crossing between two marches
struct Point
{
  double x;
  double y;

  // create a new 2D point
  Point(double x_, double y_)
  : x(x_), y(y_)
  {}

  // distance between two points
  double distance(const Point& other) const
  {
    auto diffX = x - other.x;
    auto diffY = y - other.y;
    return sqrt(diffX * diffX + diffY * diffY);
  }
};

// how many days does it take to walk from the first to the last point, considering "speed" of the surfaces
double duration(const std::vector<Point>& points)
{
  // leagues travelled per day between point[i] and point[i+1]
  static const std::vector<double> speed = { 10, 9, 8, 7, 6, 5, 10 };

  double result = 0;
  // for each surface: determine length and divide by speed
  for (size_t i = 0; i < speed.size(); i++)
  {
    auto way = points[i].distance(points[i+1]);
    result += way / speed[i];
  }
  return result;
}

// a simple pseudo-random number generator
// (produces the same result no matter what compiler you have - unlike rand() from math.h)
unsigned int myrand()
{
  static unsigned long long seed = 0;
  seed = 6364136223846793005ULL * seed + 1;
  return (unsigned int)(seed >> 30);
}

// try to move a single point's y-value by delta
// if the mutation is better, then keep the result
// if the mutation is worse,  then undo the change
void mutate(std::vector<Point>& points, double delta)
{
  auto oldDuration = duration(points);

  // decide at random whether to add or subtract
  if (myrand() & 1)
    delta = -delta;

  // change one random y-coordinate (must not be the first or last)
  auto id = myrand() % 6 + 1;
  points[id].y += delta;

  // not faster ? => undo change
  if (duration(points) >= oldDuration)
    points[id].y -= delta;
}

int main()
{
  // A
  std::vector<Point> points;
  points.push_back({ 0, 0 });

  // rotate the coordinate system by 45 degrees,
  // so that travelling one league is equivalent to moving 1/scaling units along x-axis and 1/scaling units along y-axis
  double scaling = sqrt(2);

  // when travelling the shortest distance between A and B, how wide is the marsh ?
  auto direct = 50 * scaling; // c^2 = a^2 + b^2 where a=b=50

  // entering the marsh
  auto current = ((100 - direct) / 2) / scaling; // same as 25*(sqrt(2)-1)
  points.push_back({ current, current });

  // all 5 zones of the marsh
  for (auto i = 1; i <= 5; i++)
    points.push_back({ current + i * 10, current + i * 10 });

  // B
  points.push_back({ 100 / scaling, 100 / scaling });

  // randomly mutate the crossing
  const auto NumIterations = 10000;
  for (auto delta = 0.01; delta >= 0.00001; delta /= 10)
    for (auto i = 0; i < NumIterations; i++)
      mutate(points, delta);

  // display result with 10 digits after the decimal point
  std::cout << std::fixed << std::setprecision(10)
            << duration(points) << std::endl;
  return 0;
}
