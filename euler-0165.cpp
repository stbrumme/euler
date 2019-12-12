// ////////////////////////////////////////////////////////
// # Title
// Intersections
//
// # URL
// https://projecteuler.net/problem=165
// http://euler.stephan-brumme.com/165/
//
// # Problem
// A segment is uniquely defined by its two endpoints.
// By considering two line segments in plane geometry there are three possibilities:
// the segments have zero points, one point, or infinitely many points in common.
//
// Moreover when two segments have exactly one point in common it might be the case that that common point is an endpoint of either one of the segments or of both.
// If a common point of two segments is not an endpoint of either of the segments it is an interior point of both segments.
// We will call a common point `T` of two segments `L_1` and `L_2` a true intersection point of `L_1` and `L_2` if `T` is the only common point of `L_1` and `L_2` and `T` is an interior point of both segments.
//
// Consider the three segments `L_1`, `L_2`, and `L_3`:
//
// `L_1`: (27, 44) to (12, 32)
// `L_2`: (46, 53) to (17, 62)
// `L_3`: (46, 70) to (22, 40)
//
// It can be verified that line segments `L_2` and `L_3` have a true intersection point.
// We note that as the one of the end points of `L_3`: (22,40) lies on `L_1` this is not considered to be a true point of intersection.
// `L_1` and `L_2` have no common point. So among the three line segments, we find one true intersection point.
//
// Now let us do the same for 5000 line segments. To this end, we generate 20000 numbers using the so-called "Blum Blum Shub" pseudo-random number generator.
//
// `s_0 = 290797`
// `s_{n+1} == s_n * s_n mod 50515093`
// `t_n == s_n mod 500`
//
// To create each line segment, we use four consecutive numbers tn. That is, the first line segment is given by:
//
// `(t_1, t_2)` to `(t_3, t_4)`
//
// The first four numbers computed according to the above generator should be: 27, 144, 12 and 232. The first segment would thus be (27,144) to (12,232).
//
// How many distinct true intersection points are found among the 5000 line segments?
//
// # Solved by
// Stephan Brumme
// June 2017
//
// # Algorithm
// I studied Computer Graphics at university and had no problem coming up with an intersection algorithm.
// Wikipedia's explanations of the use of the determinant are somehow weird and I would have trouble unterstanding it - but I already knew that from classes at university.
// The main issue inside my ''intersect'' code is numerical stability.
// These three lines reduce the number of digits so that the correct result is found:
// ''const auto Precision = 0.00000001;''
// ''where.x = round(where.x / Precision) * Precision;''
// ''where.y = round(where.y / Precision) * Precision;''
//
// The constant ''Precision'' was found by trial-and-error.
//
// The pseudo-random Blum Blum shub algorithm can be converted to a few lines of code (see ''next'').
//
// My code provides two simple classes ''Point'' and ''Segment'' to keep things organized.
// ''Point'' needs to support comparisons operators for ''std::sort'' and ''std::unique'' (weeding out duplicate intersections).
//
// ''main'' finds all (!) intersections and defers checking for duplicates until the end.
// Duplicates are eliminated as follows:
// - sort all intersections ==> identical intersections will be neighboring elements in the sorted set
// - call ''std::unique'' to move all duplicates to the end of the data container ("garbage")
// - remove all garbage points
//
// # Alternatives
// There is a big number of efficient and/or stable intersection algorithms.
// Quite interesting is the class of "Sweep-Line" algorithms but they are way more complex to implement.
//

#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

// generate pseudo-random numbers (Blum Blum Shub algorithm)
unsigned int next()
{
  static unsigned long long seed = 290797;
  seed *= seed;
  seed %= 50515093;
  return seed % 500;
}

// a 2D point
struct Point
{
  double x, y;

  // needed for std::unique
  bool operator==(const Point& other) const
  {
    return x == other.x && y == other.y;
  }
  // needed for std::sort
  bool operator< (const Point& other) const
  {
    if (x != other.x)
      return x < other.x;
    else
      return y < other.y;
  }
};

// define a segment
struct Segment
{
  Point from, to;
};

// find intersection of two segments, out parameter "where" is only valid if function returns true
bool intersect(const Segment& segment1, const Segment& segment2, Point& where)
{
  // shorter names for the four endpoints
  auto a = segment1.from;
  auto b = segment1.to;
  auto c = segment2.from;
  auto d = segment2.to;

  // store slope in a Point (just because I'm lazy and don't want to introduce another data type)
  Point slope1, slope2;
  slope1.x = b.x - a.x;
  slope1.y = b.y - a.y;
  slope2.x = d.x - c.x;
  slope2.y = d.y - c.y;

  auto determinant = slope1.x * slope2.y - slope2.x * slope1.y;
  // parallel ?
  if (determinant == 0)
    return false;

  // now the lines intersect, but not necessarily the segments
  auto s = (slope1.x * (a.y - c.y) - slope1.y * (a.x - c.x)) / determinant;
  auto t = (slope2.x * (a.y - c.y) - slope2.y * (a.x - c.x)) / determinant;

  // parameters s and t must be in (0 ... 1)
  // borders (=endpoints) are not true intersections according to problem statement
  if (s <= 0 || s >= 1 || t <= 0 || t >= 1)
    return false;

  // yes, intersection found (might be a duplicate, though !)
  where.x = a.x + t * slope1.x;
  where.y = a.y + t * slope1.y;

  // cut off a few digits to avoid rounding issues
  const auto Precision = 0.00000001;
  where.x = round(where.x / Precision) * Precision;
  where.y = round(where.y / Precision) * Precision;

  return true;
}

int main()
{
  std::vector<Segment> segments;
  std::vector<Point>   intersections;

  unsigned int limit = 5000;
  std::cin >> limit;

  for (unsigned int i = 0; i < limit; i++)
  {
    // create "random" segment
    Segment current;
    current.from.x = next();
    current.from.y = next();
    current.to  .x = next();
    current.to  .y = next();

    // try to intersect with all other segments
    Point where;
    for (auto compare : segments)
      if (intersect(current, compare, where))
        intersections.push_back(where);

    // add current segment to list of segments
    segments.push_back(current);
  }

  // eliminate duplicate intersection points
  std::sort(intersections.begin(), intersections.end());
  auto garbage = std::unique(intersections.begin(), intersections.end());
  intersections.erase(garbage, intersections.end());

  // display result
  std::cout << intersections.size() << std::endl;
  return 0;
}
