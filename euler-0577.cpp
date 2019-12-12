// ////////////////////////////////////////////////////////
// # Title
// Counting hexagons
//
// # URL
// https://projecteuler.net/problem=577
// http://euler.stephan-brumme.com/577/
//
// # Problem
// An equilateral triangle with integer side length `n >= 3` is divided into `n^2` equilateral triangles with side length 1 as shown in the diagram below.
// The vertices of these triangles constitute a triangular lattice with `dfrac{(n+1)(n+2)}{2}` lattice points.
//
// Let `H(n)` be the number of all regular hexagons that can be found by connecting 6 of these points.
//
// ![hexagons](p577_counting_hexagons.png)
//
// For example, `H(3)=1`, `H(6)=12` and `H(20)=966`.
//
// Find `sum_{n=3}^12345{H(n)}`.
//
// # Solved by
// Stephan Brumme
// December 2017
//
// # Algorithm
// My initial brute-force algorithm works as follows:
// - assume that every hexagon's center is a lattice point, too (bold assumption - but seems to works for all `H(n)`)
// - for each such lattice point, compute the distances to every other lattice point
// - compute the angle as well
// - keep a data structure of all distances and angles for each lattice point
// - round values to avoid errors due to numeric imprecision
//
// A hexagon can be identified by finding:
// - six points with the same distance
// - their angles are 60 degrees apart from each other
//
// The ''bruteForce()'' function computes `H(20) = 966` in a few milliseconds. For `n > 100` it becomes pretty slow, though.
//
// I entered the values `H(3)`, `H(4)`, `H(5)`, etc. (1,3,6,12,21,33,51,75,105,145,195,255,...) in http://www.oeis.org and found the sequence OEIS A011779.
// Unfortunately I couldn't handle the series expansion in a few lines of C++ code.
// Since the "larger" hexagons appear when `n mod 3 == 0` I split the sequence into three parts:
// - `H(3)`, `H(6)`, `H( 9)`, etc. (1,12,51,145,...)  ==> OEIS A236770:
// - `H(4)`, `H(7)`, `H(10)`, etc. (3,21,75,195,...)  ==> OEIS A228317:
// - `H(5)`, `H(8)`, `H(11)`, etc. (6,33,105,255,...)
//
// Even though I found sequences with simple formulas for `n mod 3 == 0` and `n mod 3 == 1`, there was no such sequence for `n mod 3 == 2`.
// However, I noticed that every `H(n)` for `n mod 3 == 2` is a multiple of 3. And voila: 2,11,35,85 is OEIS A000914.
//
// In the end I have a simple loop from 3 to 12345 that calls ''A236770()'', ''A228317()'' or ''A000914()'', depending on ''n % 3''.
//
// # Note
// Honestly, I thought that my ''bruteForce()'' results might be wrong even for small ''n'' because of numeric imprecisions.
// But the code proved to be quite stable: all `H(n)` for `n <= 100` are correct (I didn't verify larger `n`).
//
// Today is the end of the year - and the end of the fiscal year, too, so I donated 10 bucks to OEIS. For tax reasons :-)

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>

// ---------- brute force up to n=100 ----------

const auto PI = 3.14159265358979323846264;
const auto Precision = 0.00001;

// 2D point
struct Point
{
  double x, y;
  Point(double x_, double y_)
  : x(x_), y(y_)
  { }

  // distance between two points
  double distance(const Point& other) const
  {
    auto dx = other.x - x;
    auto dy = other.y - y;
    return sqrt(dx*dx + dy * dy); // I could skip sqrt because the squared distance works as well
  }

  // angle (in degrees, always positive)
  double angle(const Point& other) const
  {
    auto dx = other.x - x;
    auto dy = other.y - y;
    auto radian = atan2(dy, dx);

    // convert to degrees
    auto degrees = radian * 180 / PI;
    if (degrees < 0)
      degrees += 360;
    return degrees;
  }
};

// round a double with a certain precision, e.g. round(5.126, 0.01) = 5.13
double round(double a, double precision)
{
  return round(a / precision) * precision;
}

// look for hexagons using polar coordinates
unsigned long long bruteForce(unsigned int size)
{
  // horizontal and vertical distance between two neighbors in the lattice / grid
  double dx = 1;
  double dy = sqrt(3.0) / 2; // height of https://en.wikipedia.org/wiki/Equilateral_triangle

  // generate all points
  std::vector<Point> points;
  auto numPoints = (size + 1) * (size + 2) / 2;
  for (unsigned int gridY = 0; gridY <= size; gridY++)
  {
    // position of the left-most point in each row
    double y = gridY * dy;
    double x = gridY * dx / 2;

    // all points in the current row
    auto width = size - gridY;
    for (unsigned int gridX = 0; gridX <= width; gridX++)
    {
      points.push_back(Point(x, y));
      x += dx;
    }
  }

  // for each points: compute polar coordinates of the vector to each other point
  std::vector<std::map<double, std::vector<double>>> polar; // map: distance => angles
  for (auto i : points)
  {
    // add one element
    polar.resize(polar.size() + 1);
    // direct access to it
    auto& current = polar.back();
    for (auto j : points)
    {
      auto distance = i.distance(j);
      auto angle    = i.angle(j);

      // avoid precision issues
      distance = round(distance, Precision);
      angle    = round(angle,    Precision);

      if (distance > 0)
        current[distance].push_back(angle);
    }
  }

  // find hexagons amidst these polar coordinates:
  // six points having the same distance where angles are (2*pi) / 6 apart
  unsigned long long numFound = 0;
  for (unsigned int point = 0; point < numPoints; point++)
  {
    for (auto& distance : polar[point])
    {
      auto& candidates = distance.second;
      // hexagon impossible ?
      if (candidates.size() < 6)
        continue;

      // look for subsets with 6 elements { start,     start+60,  start+120,
      //                                    start+180, start+240, start+300 }
      std::sort(candidates.begin(), candidates.end());
      for (auto start : candidates)
      {
        // smallest angle must be 0 <= start < 60 (in degrees)
        if (start >= 60) // hexagon has 6 sides, so 360 / 6 = 60
          break;

        // check if angle+60, +120, +180, ... exist
        bool valid = true;
        for (auto next = start + 60; next < 360; next += 60)
          // note: need the same rounding as used before !
          if (!std::binary_search(candidates.begin(), candidates.end(), round(next, Precision)))
          {
            valid = false;
            break;
          }

        // yes, found another hexagon
        if (valid)
          numFound++;
      }
    }
  }

  return numFound;
}

// ---------- super-fast algorithm ----------

// sequence https://oeis.org/A000914
unsigned long long A000914(unsigned long long n)
{
  return n * (n+1) * (n+2) * (3*n+5) / 24;
}

// sequence https://oeis.org/A228317
unsigned long long A228317(unsigned long long n)
{
  return n * (n-1) * (n-2) * (3*n-5) /  8;
}

// sequence https://oeis.org/A236770
unsigned long long A236770(unsigned long long n)
{
  return n * (n+1) * (3*n*n + 3*n - 2) / 8;
}

int main()
{
  unsigned int size = 12345; // n
  std::cin >> size;

  // brute force
  //std::cout << bruteForce(size) << std::endl;

  // indices for A000914, A228317 and A236770
  unsigned int i000914 = 1;
  unsigned int i228317 = 3;
  unsigned int i236770 = 1;

  unsigned long long sum = 0;
  // no hexagons below n = 3
  for (unsigned int n = 3; n <= size; n++)
  {
    unsigned long long fast = 0;
    // method of calculation depends on n mod 3
    switch (n % 3)
    {
    case 0:
      fast = A236770(i236770++);
      break;
    case 1:
      fast = A228317(i228317++);
      break;
    case 2:
      fast = 3 * A000914(i000914++);
      break;
    }

    sum += fast;
  }

  // that's it !
  std::cout << sum << std::endl;
  return 0;
}
